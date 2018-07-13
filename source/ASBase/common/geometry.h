#pragma once

#include <vector>

class QSize;

namespace aspia {

class Point
{
public:
    constexpr Point() : Point(0, 0) {}
    constexpr Point(int xpos, int ypos)
        : xp(xpos), yp(ypos)
    {
    }

    constexpr inline bool isNull() const;

    constexpr inline int x() const { return xp; }
    constexpr inline int y() const { return yp; }
    constexpr inline void setX(int x);
    constexpr inline void setY(int y);

    constexpr inline int manhattanLength() const;

    constexpr inline int &rx();
    constexpr inline int &ry();

    constexpr inline Point &operator+=(const Point &p);
    constexpr inline Point &operator-=(const Point &p);

    constexpr inline Point &operator*=(float factor);
    constexpr inline Point &operator*=(double factor);
    constexpr inline Point &operator*=(int factor);

    //constexpr inline Point &operator/=(qreal divisor);

    constexpr static inline int dotProduct(const Point &p1, const Point &p2)
    {
        return p1.xp * p2.xp + p1.yp * p2.yp;
    }

    friend constexpr inline bool operator==(const Point &, const Point &);
    friend constexpr inline bool operator!=(const Point &, const Point &);
    friend constexpr inline const Point operator+(const Point &, const Point &);
    friend constexpr inline const Point operator-(const Point &, const Point &);
    friend constexpr inline const Point operator*(const Point &, float);
    friend constexpr inline const Point operator*(float, const Point &);
    friend constexpr inline const Point operator*(const Point &, double);
    friend constexpr inline const Point operator*(double, const Point &);
    friend constexpr inline const Point operator*(const Point &, int);
    friend constexpr inline const Point operator*(int, const Point &);
    friend constexpr inline const Point operator+(const Point &);
    friend constexpr inline const Point operator-(const Point &);
    //friend constexpr inline const Point operator/(const Point &, qreal);

private:
    int xp;
    int yp;
};

class Size
{
public:
    constexpr Size() noexcept : Size(0, 0) {}
    constexpr Size(int w, int h) noexcept : wd(w), ht(h) {}
    constexpr Size(const QSize &) noexcept;

    constexpr inline bool isNull() const noexcept;
    constexpr inline bool isEmpty() const noexcept;
    constexpr inline bool isValid() const noexcept;

    constexpr inline int width() const noexcept { return wd; }
    constexpr inline int height() const noexcept { return ht; }
    constexpr inline void setWidth(int w) noexcept;
    constexpr inline void setHeight(int h) noexcept;
    void transpose() noexcept;
    constexpr inline Size transposed() const noexcept;

    constexpr inline Size expandedTo(const Size &) const noexcept;
    constexpr inline Size boundedTo(const Size &) const noexcept;

    constexpr inline int &rwidth() noexcept;
    constexpr inline int &rheight() noexcept;

    constexpr inline Size &operator+=(const Size &) noexcept;
    constexpr inline Size &operator-=(const Size &) noexcept;

    friend inline constexpr bool operator==(const Size &, const Size &) noexcept;
    friend inline constexpr bool operator!=(const Size &, const Size &) noexcept;
    friend inline constexpr const Size operator+(const Size &, const Size &) noexcept;
    friend inline constexpr const Size operator-(const Size &, const Size &) noexcept;

private:
    int wd;
    int ht;
};

class ASPIA_BASE_API Rect
{
public:
    constexpr Rect() noexcept : x1(0), y1(0), x2(-1), y2(-1) {}
    constexpr Rect(const Point &topleft, const Point &bottomright) noexcept;
    constexpr Rect(const Point &topleft, const Size &size) noexcept;
    constexpr Rect(int left, int top, int width, int height) noexcept
        : x1(left), y1(top - height), x2(left + width), y2(top)
    {}

    constexpr inline bool isNull() const noexcept;
    constexpr inline bool isEmpty() const noexcept;
    constexpr inline bool isValid() const noexcept;

    constexpr inline int left() const noexcept;
    constexpr inline int top() const noexcept;
    constexpr inline int right() const noexcept;
    constexpr inline int bottom() const noexcept;
    Rect normalized() const noexcept;

    constexpr inline int x() const noexcept { return x1; }
    constexpr inline int y() const noexcept { return y1; }
    constexpr inline void setLeft(int pos) noexcept;
    constexpr inline void setTop(int pos) noexcept;
    constexpr inline void setRight(int pos) noexcept;
    constexpr inline void setBottom(int pos) noexcept;
    constexpr inline void setX(int x) noexcept;
    constexpr inline void setY(int y) noexcept;

    constexpr inline void setTopLeft(const Point &p) noexcept;
    constexpr inline void setBottomRight(const Point &p) noexcept;
    constexpr inline void setTopRight(const Point &p) noexcept;
    constexpr inline void setBottomLeft(const Point &p) noexcept;

    constexpr inline Point topLeft() const noexcept;
    constexpr inline Point bottomRight() const noexcept;
    constexpr inline Point topRight() const noexcept;
    constexpr inline Point bottomLeft() const noexcept;
    constexpr inline Point center() const noexcept;

    constexpr inline void moveLeft(int pos) noexcept;
    constexpr inline void moveTop(int pos) noexcept;
    constexpr inline void moveRight(int pos) noexcept;
    constexpr inline void moveBottom(int pos) noexcept;
    constexpr inline void moveTopLeft(const Point &p) noexcept;
    constexpr inline void moveBottomRight(const Point &p) noexcept;
    constexpr inline void moveTopRight(const Point &p) noexcept;
    constexpr inline void moveBottomLeft(const Point &p) noexcept;
    constexpr inline void moveCenter(const Point &p) noexcept;

    constexpr inline void translate(int dx, int dy) noexcept;
    constexpr inline void translate(const Point &p) noexcept;
    constexpr inline Rect translated(int dx, int dy) const noexcept;
    constexpr inline Rect translated(const Point &p) const noexcept;
    constexpr inline Rect transposed() const noexcept;

    constexpr inline void moveTo(int x, int t) noexcept;
    constexpr inline void moveTo(const Point &p) noexcept;

    constexpr inline void setRect(int x, int y, int w, int h) noexcept;
    constexpr inline void getRect(int *x, int *y, int *w, int *h) const;

    constexpr inline void setCoords(int x1, int y1, int x2, int y2) noexcept;
    constexpr inline void getCoords(int *x1, int *y1, int *x2, int *y2) const;

    constexpr inline void adjust(int x1, int y1, int x2, int y2) noexcept;
    constexpr inline Rect adjusted(int x1, int y1, int x2, int y2) const noexcept;

    constexpr inline Size size() const noexcept;
    constexpr inline int width() const noexcept;
    constexpr inline int height() const noexcept;
    constexpr inline void setWidth(int w) noexcept;
    constexpr inline void setHeight(int h) noexcept;
    constexpr inline void setSize(const Size &s) noexcept;

    Rect operator|(const Rect &r) const noexcept;
    Rect operator&(const Rect &r) const noexcept;
    inline Rect& operator|=(const Rect &r) noexcept;
    inline Rect& operator&=(const Rect &r) noexcept;

    bool contains(const Rect &r, bool proper = false) const noexcept;
    bool contains(const Point &p, bool proper = false) const noexcept;
    inline bool contains(int x, int y) const noexcept;
    inline bool contains(int x, int y, bool proper) const noexcept;
    inline Rect united(const Rect &other) const noexcept;
    inline Rect intersected(const Rect &other) const noexcept;
    bool intersects(const Rect &r) const noexcept;

    friend constexpr inline bool operator==(const Rect &, const Rect &) noexcept;
    friend constexpr inline bool operator!=(const Rect &, const Rect &) noexcept;

private:
    int x1;
    int y1;
    int x2;
    int y2;
};

struct ASPIA_BASE_API RegionPrivate
{
    int numRects;
    int innerArea;
    std::vector<Rect> rects;
    Rect extents;
    Rect innerRect;

    inline RegionPrivate() : numRects(0), innerArea(-1) {}
    inline RegionPrivate(const Rect &r)
        : numRects(1),
        innerArea(r.width() * r.height()),
        extents(r),
        innerRect(r)
    {
    }

    void intersect(const Rect &r);

    /*
    * Returns \c true if r is guaranteed to be fully contained in this region.
    * A false return value does not guarantee the opposite.
    */
    inline bool contains(const RegionPrivate &r) const {
        return contains(r.extents);
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

    const Rect *begin() const noexcept
    {
        return numRects == 1 ? &extents : rects.data();
    } // avoid vectorize()

    const Rect *end() const noexcept
    {
        return begin() + numRects;
    }

    inline void append(const Rect *r);
    void append(const RegionPrivate *r);
    void prepend(const Rect *r);
    void prepend(const RegionPrivate *r);
    inline bool canAppend(const Rect *r) const;
    inline bool canAppend(const RegionPrivate *r) const;
    inline bool canPrepend(const Rect *r) const;
    inline bool canPrepend(const RegionPrivate *r) const;

    inline bool mergeFromRight(Rect *left, const Rect *right);
    inline bool mergeFromLeft(Rect *left, const Rect *right);
    inline bool mergeFromBelow(Rect *top, const Rect *bottom,
        const Rect *nextToTop,
        const Rect *nextToBottom);
    inline bool mergeFromAbove(Rect *bottom, const Rect *top,
        const Rect *nextToBottom,
        const Rect *nextToTop);
};

class ASPIA_BASE_API Region
{
public:
    enum RegionType { Rectangle, Ellipse };

    Region();
    Region(int x, int y, int w, int h, RegionType t = Rectangle);
    Region(const Rect &r, RegionType t = Rectangle);
    //Region(const QPolygon &pa, Qt::FillRule fillRule = Qt::OddEvenFill);
    Region(const Region &region);
    Region(Region &&other) noexcept
        : d(other.d) {
        other.d = const_cast<RegionPrivate*>(&shared_empty);
    }
    //Region(const QBitmap &bitmap);
    ~Region();
    Region &operator=(const Region &);
#ifdef Q_COMPILER_RVALUE_REFS
    inline Region &operator=(Region &&other) noexcept
    {
        qSwap(d, other.d); return *this;
    }
#endif
    //inline void swap(Region &other) noexcept { qSwap(d, other.d); }
    bool isEmpty() const;
    bool isNull() const;

    typedef const Rect *const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    const_iterator begin()  const noexcept;
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator end()    const noexcept;
    const_iterator cend()   const noexcept { return end(); }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator rend()    const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend()   const noexcept { return rend(); }

    bool contains(const Point &p) const;
    bool contains(const Rect &r) const;

    void translate(int dx, int dy);
    inline void translate(const Point &p) { translate(p.x(), p.y()); }
    Region translated(int dx, int dy) const;
    inline Region translated(const Point &p) const { return translated(p.x(), p.y()); }

    Region united(const Region &r) const;
    Region united(const Rect &r) const;
    Region intersected(const Region &r) const;
    Region intersected(const Rect &r) const;
    Region subtracted(const Region &r) const;
    Region xored(const Region &r) const;

    bool intersects(const Region &r) const;
    bool intersects(const Rect &r) const;

    Rect boundingRect() const noexcept;
    void setRects(const Rect *rect, int num);
    int rectCount() const noexcept;
#ifdef Q_COMPILER_MANGLES_RETURN_TYPE
    // ### Qt 6: remove these, they're kept for MSVC compat
    const Region operator|(const Region &r) const;
    const Region operator+(const Region &r) const;
    const Region operator+(const QRect &r) const;
    const Region operator&(const Region &r) const;
    const Region operator&(const QRect &r) const;
    const Region operator-(const Region &r) const;
    const Region operator^(const Region &r) const;
#else
    Region operator|(const Region &r) const;
    Region operator+(const Region &r) const;
    Region operator+(const Rect &r) const;
    Region operator&(const Region &r) const;
    Region operator&(const Rect &r) const;
    Region operator-(const Region &r) const;
    Region operator^(const Region &r) const;
#endif // Q_COMPILER_MANGLES_RETURN_TYPE
    Region& operator|=(const Region &r);
    Region& operator+=(const Region &r);
    Region& operator+=(const Rect &r);
    Region& operator&=(const Region &r);
    Region& operator&=(const Rect &r);
    Region& operator-=(const Region &r);
    Region& operator^=(const Region &r);

    bool operator==(const Region &r) const;
    inline bool operator!=(const Region &r) const { return !(operator==(r)); }
    //operator QVariant() const;

private:
    Region copy() const;   // helper of detach.
    void detach();
    friend struct QRegionPrivate;

    RegionPrivate *d;
    static const struct RegionPrivate shared_empty;
    static void cleanUp(RegionPrivate *x);
};

} // namespace aspia
