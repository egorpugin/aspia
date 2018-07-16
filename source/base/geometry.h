#pragma once

#include <algorithm>
#include <deque>
#include <vector>

namespace aspia {

struct Point
{
    int xp;
    int yp;

    constexpr Point() : Point(0, 0) {}
    constexpr Point(int xpos, int ypos)
        : xp(xpos), yp(ypos)
    {
    }

    constexpr inline int x() const { return xp; }
    constexpr inline int y() const { return yp; }

    bool operator==(const Point &s) const
    {
        return xp == s.xp && yp == s.yp;
    }
    bool operator!=(const Point &s) const
    {
        return !operator==(s);
    }
};

struct Size
{
    int wd;
    int ht;

    constexpr Size() noexcept : Size(0, 0) {}
    constexpr Size(int w, int h) noexcept : wd(w), ht(h) {}

    constexpr inline int width() const noexcept { return wd; }
    constexpr inline int height() const noexcept { return ht; }

    bool operator==(const Size &s) const
    {
        return wd == s.wd && ht == s.ht;
    }
    bool operator!=(const Size &s) const
    {
        return !operator==(s);
    }
};

struct Rect
{
    int x1;
    int y1;
    int x2;
    int y2;

    Rect() : x1(0), y1(0), x2(-1), y2(-1) {}
    Rect(const Point &topleft, const Size &size)
        : Rect(topleft.xp, topleft.yp, size.wd, size.ht)
    {
    }
    constexpr Rect(int left, int top, int width, int height)
        : x1(left), y1(top - height), x2(left + width), y2(top)
    {}

    constexpr inline int x() const noexcept { return x1; }
    constexpr inline int y() const noexcept { return y1; }

    constexpr inline int width() const noexcept { return x2-x1; }
    constexpr inline int height() const noexcept { return y2-y1; }

    constexpr inline int left() const noexcept { return x1; }
    constexpr inline int top() const noexcept { return y2; }
    constexpr inline int right() const noexcept { return x2; }
    constexpr inline int bottom() const noexcept { return y1; }

    constexpr inline Size size() const noexcept
    {
        return {x2-x1,y2-y1};
    }

    inline Rect intersected(const Rect &other) const noexcept
    {
        return *this & other;
    }

    Rect operator&(const Rect &r) const
    {
        int l1 = x1;
        int r1 = x1;
        if (x2 - x1 + 1 < 0)
            l1 = x2;
        else
            r1 = x2;

        int l2 = r.x1;
        int r2 = r.x1;
        if (r.x2 - r.x1 + 1 < 0)
            l2 = r.x2;
        else
            r2 = r.x2;

        if (l1 > r2 || l2 > r1)
            return Rect();

        int t1 = y1;
        int b1 = y1;
        if (y2 - y1 + 1 < 0)
            t1 = y2;
        else
            b1 = y2;

        int t2 = r.y1;
        int b2 = r.y1;
        if (r.y2 - r.y1 + 1 < 0)
            t2 = r.y2;
        else
            b2 = r.y2;

        if (t1 > b2 || t2 > b1)
            return Rect();

        Rect tmp;
        tmp.x1 = std::max(l1, l2);
        tmp.x2 = std::max(r1, r2);
        tmp.y1 = std::max(t1, t2);
        tmp.y2 = std::max(b1, b2);
        return tmp;
    }

    bool operator==(const Rect &s) const
    {
        return std::tie(x1, y1, x2, y2) == std::tie(s.x1, s.y1, s.x2, s.y2);
    }
    bool operator!=(const Rect &s) const
    {
        return !operator==(s);
    }

    inline bool isEmpty() const
    {
        return x1 > x2 || y1 > y2;
    }

    inline void setCoords(int xp1, int yp1, int xp2, int yp2)
    {
        x1 = xp1;
        y1 = yp1;
        x2 = xp2;
        y2 = yp2;
    }
};

struct Region
{
    int numRects{ 0 };
    int innerArea{ -1 };
    std::deque<Rect> rects;
    Rect extents;
    Rect innerRect;

    Region() {}
    Region(int x, int y, int w, int h)
        : Region(Rect{x,y,w,h})
    {
    }
    Region(const Rect &r)
        : numRects(1),
        innerArea(r.width() * r.height()),
        extents(r),
        innerRect(r)
    {
    }

    bool operator==(const Region &s) const
    {
        if (numRects != s.numRects) {
            return false;
        }
        else if (numRects == 0) {
            return true;
        }
        else if (extents != s.extents) {
            return false;
        }
        else if (numRects == 1 && s.numRects == 1) {
            return true; // equality tested in previous if-statement
        }
        else {
            const Rect *rr1 = (numRects == 1) ? &extents : &rects[0];
            const Rect *rr2 = (s.numRects == 1) ? &s.extents : s.&rects[0];
            for (int i = 0; i < numRects; ++i, ++rr1, ++rr2) {
                if (*rr1 != *rr2)
                    return false;
            }
        }

        return true;
    }
    bool operator!=(const Region &s) const
    {
        return !operator==(s);
    }

    Region &operator+=(const Rect &r)
    {
        if (isEmpty())
            return *this = r;
        if (r.isEmpty())
            return *this;

        if (contains(r)) {
            return *this;
        }
        else if (within(r)) {
            return *this = r;
        }
        else if (canAppend(&r)) {
            append(&r);
            return *this;
        }
        else if (canPrepend(&r)) {
            prepend(&r);
            return *this;
        }
        else if (numRects == 1 && extents == r) {
            return *this;
        }
        else {
            Region p(r);
            UnionRegion(this, &p, *this);
            return *this;
        }
    }

private:
    bool isEmpty() const
    {
        return numRects == 0;
    }

    inline bool contains(const Rect &r2) const {
        const Rect &r1 = innerRect;
        return r2.left() >= r1.left() && r2.right() <= r1.right()
            && r2.top() >= r1.top() && r2.bottom() <= r1.bottom();
    }

    /*
    * Returns \c true if this region is guaranteed to be fully contained in r.
    */
    inline bool within(const Rect &r1) const {
        const Rect &r2 = extents;
        return r2.left() >= r1.left() && r2.right() <= r1.right()
            && r2.top() >= r1.top() && r2.bottom() <= r1.bottom();
    }

    bool canAppend(const Rect *r) const
    {
        const Rect *myLast = (numRects == 1) ? &extents : (&rects[0] + (numRects - 1));
        if (r->top() > myLast->bottom())
            return true;
        if (r->top() == myLast->top()
            && r->height() == myLast->height()
            && r->left() > myLast->right())
        {
            return true;
        }

        return false;
    }

    bool canPrepend(const Rect *r) const
    {
        const Rect *myFirst = (numRects == 1) ? &extents : &rects[0];
        if (r->bottom() < myFirst->top()) // not overlapping
            return true;
        if (r->top() == myFirst->top()
            && r->height() == myFirst->height()
            && r->right() < myFirst->left())
        {
            return true;
        }

        return false;
    }

    inline void updateInnerRect(const Rect &rect) {
        const int area = rect.width() * rect.height();
        if (area > innerArea) {
            innerArea = area;
            innerRect = rect;
        }
    }

    inline void vectorize() {
        if (numRects == 1) {
            if (!rects.size())
                rects.resize(1);
            rects[0] = extents;
        }
    }

    void append(const Rect *r)
    {
        Rect *myLast = (numRects == 1 ? &extents : &rects[0] + (numRects - 1));
        if (mergeFromRight(myLast, r)) {
            if (numRects > 1) {
                const Rect *nextToTop = (numRects > 2 ? myLast - 2 : 0);
                if (mergeFromBelow(myLast - 1, myLast, nextToTop, 0))
                    --numRects;
            }
        }
        else if (mergeFromBelow(myLast, r, (numRects > 1 ? myLast - 1 : 0), 0)) {
            // nothing
        }
        else {
            vectorize();
            ++numRects;
            updateInnerRect(*r);
            if (rects.size() < numRects)
                rects.resize(numRects);
            rects[numRects - 1] = *r;
        }
        extents.setCoords(std::min(extents.left(), r->left()),
            std::min(extents.top(), r->top()),
            std::max(extents.right(), r->right()),
            std::max(extents.bottom(), r->bottom()));
    }

    void prepend(const Rect *r)
    {
        Rect *myFirst = (numRects == 1 ? &extents : &rects[0]);
        if (mergeFromLeft(myFirst, r)) {
            if (numRects > 1) {
                const Rect *nextToFirst = (numRects > 2 ? myFirst + 2 : 0);
                if (mergeFromAbove(myFirst + 1, myFirst, nextToFirst, 0)) {
                    --numRects;
                    memmove(&rects[0], &rects[0] + 1,
                        numRects * sizeof(Rect));
                }
            }
        }
        else if (mergeFromAbove(myFirst, r, (numRects > 1 ? myFirst + 1 : 0), 0)) {
            // nothing
        }
        else {
            vectorize();
            ++numRects;
            updateInnerRect(*r);
            rects.push_front(*r);
        }
        extents.setCoords(std::min(extents.left(), r->left()),
            std::min(extents.top(), r->top()),
            std::max(extents.right(), r->right()),
            std::max(extents.bottom(), r->bottom()));
    }

    static void UnionRegion(const Region *reg1, const Region *reg2, Region &dest)
    {
        if (reg1->innerArea > reg2->innerArea) {
            dest.innerArea = reg1->innerArea;
            dest.innerRect = reg1->innerRect;
        }
        else {
            dest.innerArea = reg2->innerArea;
            dest.innerRect = reg2->innerRect;
        }
        miRegionOp(dest, reg1, reg2, miUnionO, miUnionNonO, miUnionNonO);

        dest.extents.setCoords(std::min(reg1->extents.left(), reg2->extents.left()),
            std::min(reg1->extents.top(), reg2->extents.top()),
            std::max(reg1->extents.right(), reg2->extents.right()),
            std::max(reg1->extents.bottom(), reg2->extents.bottom()));
    }

    static void miRegionOp(Region &dest,
        const Region *reg1, const Region *reg2,
        OverlapFunc overlapFunc, NonOverlapFunc nonOverlap1Func,
        NonOverlapFunc nonOverlap2Func)
    {
        const QRect *r1;         // Pointer into first region
        const QRect *r2;         // Pointer into 2d region
        const QRect *r1End;               // End of 1st region
        const QRect *r2End;               // End of 2d region
        int ybot;          // Bottom of intersection
        int ytop;          // Top of intersection
        int prevBand;               // Index of start of previous band in dest
        int curBand;                // Index of start of current band in dest
        const QRect *r1BandEnd;  // End of current band in r1
        const QRect *r2BandEnd;  // End of current band in r2
        int top;                    // Top of non-overlapping band
        int bot;                    // Bottom of non-overlapping band

                                    /*
                                    * Initialization:
                                    *  set r1, r2, r1End and r2End appropriately, preserve the important
                                    * parts of the destination region until the end in case it's one of
                                    * the two source regions, then mark the "new" region empty, allocating
                                    * another array of rectangles for it to use.
                                    */
        if (reg1->numRects == 1)
            r1 = &reg1->extents;
        else
            r1 = reg1->&rects[0];
        if (reg2->numRects == 1)
            r2 = &reg2->extents;
        else
            r2 = reg2->&rects[0];

        r1End = r1 + reg1->numRects;
        r2End = r2 + reg2->numRects;

        dest.vectorize();

        /*
        * The following calls are going to detach dest.rects. Since dest might be
        * aliasing *reg1 and/or *reg2, and we could have active iterators on
        * reg1->rects and reg2->rects (if the regions have more than 1 rectangle),
        * take a copy of dest.rects to keep those iteractors valid.
        */
        const auto destRectsCopy = dest.rects;

        dest.numRects = 0;

        /*
        * Allocate a reasonable number of rectangles for the new region. The idea
        * is to allocate enough so the individual functions don't need to
        * reallocate and copy the array, which is time consuming, yet we don't
        * have to worry about using too much memory. I hope to be able to
        * nuke the realloc() at the end of this function eventually.
        */
        dest.rects.resize(qMax(reg1->numRects, reg2->numRects) * 2);

        /*
        * Initialize ybot and ytop.
        * In the upcoming loop, ybot and ytop serve different functions depending
        * on whether the band being handled is an overlapping or non-overlapping
        * band.
        *  In the case of a non-overlapping band (only one of the regions
        * has points in the band), ybot is the bottom of the most recent
        * intersection and thus clips the top of the rectangles in that band.
        * ytop is the top of the next intersection between the two regions and
        * serves to clip the bottom of the rectangles in the current band.
        *  For an overlapping band (where the two regions intersect), ytop clips
        * the top of the rectangles of both regions and ybot clips the bottoms.
        */
        if (reg1->extents.top() < reg2->extents.top())
            ybot = reg1->extents.top() - 1;
        else
            ybot = reg2->extents.top() - 1;

        /*
        * prevBand serves to mark the start of the previous band so rectangles
        * can be coalesced into larger rectangles. qv. miCoalesce, above.
        * In the beginning, there is no previous band, so prevBand == curBand
        * (curBand is set later on, of course, but the first band will always
        * start at index 0). prevBand and curBand must be indices because of
        * the possible expansion, and resultant moving, of the new region's
        * array of rectangles.
        */
        prevBand = 0;

        do {
            curBand = dest.numRects;

            /*
            * This algorithm proceeds one source-band (as opposed to a
            * destination band, which is determined by where the two regions
            * intersect) at a time. r1BandEnd and r2BandEnd serve to mark the
            * rectangle after the last one in the current band for their
            * respective regions.
            */
            r1BandEnd = r1;
            while (r1BandEnd != r1End && r1BandEnd->top() == r1->top())
                ++r1BandEnd;

            r2BandEnd = r2;
            while (r2BandEnd != r2End && r2BandEnd->top() == r2->top())
                ++r2BandEnd;

            /*
            * First handle the band that doesn't intersect, if any.
            *
            * Note that attention is restricted to one band in the
            * non-intersecting region at once, so if a region has n
            * bands between the current position and the next place it overlaps
            * the other, this entire loop will be passed through n times.
            */
            if (r1->top() < r2->top()) {
                top = qMax(r1->top(), ybot + 1);
                bot = qMin(r1->bottom(), r2->top() - 1);

                if (nonOverlap1Func != 0 && bot >= top)
                    (*nonOverlap1Func)(dest, r1, r1BandEnd, top, bot);
                ytop = r2->top();
            }
            else if (r2->top() < r1->top()) {
                top = qMax(r2->top(), ybot + 1);
                bot = qMin(r2->bottom(), r1->top() - 1);

                if (nonOverlap2Func != 0 && bot >= top)
                    (*nonOverlap2Func)(dest, r2, r2BandEnd, top, bot);
                ytop = r1->top();
            }
            else {
                ytop = r1->top();
            }

            /*
            * If any rectangles got added to the region, try and coalesce them
            * with rectangles from the previous band. Note we could just do
            * this test in miCoalesce, but some machines incur a not
            * inconsiderable cost for function calls, so...
            */
            if (dest.numRects != curBand)
                prevBand = miCoalesce(dest, prevBand, curBand);

            /*
            * Now see if we've hit an intersecting band. The two bands only
            * intersect if ybot >= ytop
            */
            ybot = qMin(r1->bottom(), r2->bottom());
            curBand = dest.numRects;
            if (ybot >= ytop)
                (*overlapFunc)(dest, r1, r1BandEnd, r2, r2BandEnd, ytop, ybot);

            if (dest.numRects != curBand)
                prevBand = miCoalesce(dest, prevBand, curBand);

            /*
            * If we've finished with a band (y2 == ybot) we skip forward
            * in the region to the next band.
            */
            if (r1->bottom() == ybot)
                r1 = r1BandEnd;
            if (r2->bottom() == ybot)
                r2 = r2BandEnd;
        } while (r1 != r1End && r2 != r2End);

        /*
        * Deal with whichever region still has rectangles left.
        */
        curBand = dest.numRects;
        if (r1 != r1End) {
            if (nonOverlap1Func != 0) {
                do {
                    r1BandEnd = r1;
                    while (r1BandEnd < r1End && r1BandEnd->top() == r1->top())
                        ++r1BandEnd;
                    (*nonOverlap1Func)(dest, r1, r1BandEnd, qMax(r1->top(), ybot + 1), r1->bottom());
                    r1 = r1BandEnd;
                } while (r1 != r1End);
            }
        }
        else if ((r2 != r2End) && (nonOverlap2Func != 0)) {
            do {
                r2BandEnd = r2;
                while (r2BandEnd < r2End && r2BandEnd->top() == r2->top())
                    ++r2BandEnd;
                (*nonOverlap2Func)(dest, r2, r2BandEnd, qMax(r2->top(), ybot + 1), r2->bottom());
                r2 = r2BandEnd;
            } while (r2 != r2End);
        }

        if (dest.numRects != curBand)
            (void)miCoalesce(dest, prevBand, curBand);

        /*
        * A bit of cleanup. To keep regions from growing without bound,
        * we shrink the array of rectangles to match the new number of
        * rectangles in the region.
        *
        * Only do this stuff if the number of rectangles allocated is more than
        * twice the number of rectangles in the region (a simple optimization).
        */
        if (qMax(4, dest.numRects) < (dest.rects.size() >> 1))
            dest.rects.resize(dest.numRects);
    }


    bool mergeFromRight(Rect *left, const Rect *right)
    {
        if (canMergeFromRight(left, right)) {
            left->setRight(right->right());
            updateInnerRect(*left);
            return true;
        }
        return false;
    }

    bool mergeFromLeft(Rect *right, const Rect *left)
    {
        if (canMergeFromLeft(right, left)) {
            right->setLeft(left->left());
            updateInnerRect(*right);
            return true;
        }
        return false;
    }

    bool mergeFromBelow(Rect *top, const Rect *bottom,
        const Rect *nextToTop,
        const Rect *nextToBottom)
    {
        if (canMergeFromBelow(top, bottom, nextToTop, nextToBottom)) {
            top->setBottom(bottom->bottom());
            updateInnerRect(*top);
            return true;
        }
        return false;
    }

    static inline bool canMergeFromRight(const Rect *left, const Rect *right)
    {
        return (right->top() == left->top()
            && right->bottom() == left->bottom()
            && right->left() <= (left->right() + 1));
    }

    static inline bool canMergeFromLeft(const Rect *right, const Rect *left)
    {
        return canMergeFromRight(left, right);
    }

    static inline bool canMergeFromBelow(const Rect *top, const Rect *bottom,
        const QRect *nextToTop,
        const QRect *nextToBottom)
    {
        if (nextToTop && nextToTop->y() == top->y())
            return false;
        if (nextToBottom && nextToBottom->y() == bottom->y())
            return false;

        return ((top->bottom() >= (bottom->top() - 1))
            && top->left() == bottom->left()
            && top->right() == bottom->right());
    }

    bool mergeFromAbove(Rect *bottom, const Rect *top,
        const Rect *nextToBottom,
        const Rect *nextToTop)
    {
        if (canMergeFromBelow(top, bottom, nextToTop, nextToBottom)) {
            bottom->setTop(top->top());
            updateInnerRect(*bottom);
            return true;
        }
        return false;
    }





};

} // namespace aspia
