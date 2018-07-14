/*
 * Aspia: Remote desktop and file transfer tool.
 * Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "desktop_panel.h"

#include <QMenu>

#include "key_sequence_dialog.h"

namespace aspia {

DesktopPanel::DesktopPanel(proto::auth::SessionType session_type, QWidget* parent)
    : QFrame(parent)
{
    ui.setupUi(this);

    connect(ui.button_settings, &QPushButton::pressed, this, &DesktopPanel::settingsButton);
    connect(ui.button_autosize, &QPushButton::pressed, this, &DesktopPanel::onAutosizeButton);
    connect(ui.button_full_screen, &QPushButton::clicked, this, &DesktopPanel::onFullscreenButton);

    if (session_type == proto::auth::SESSION_TYPE_DESKTOP_MANAGE)
    {
        keys_menu_ = new QMenu(this);
        keys_menu_->addAction(ui.action_alt_tab);
        keys_menu_->addAction(ui.action_alt_shift_tab);
        keys_menu_->addAction(ui.action_printscreen);
        keys_menu_->addAction(ui.action_alt_printscreen);
        keys_menu_->addAction(ui.action_custom);

        connect(keys_menu_, &QMenu::aboutToShow, [this]() { allow_hide_ = false; });
        connect(keys_menu_, &QMenu::aboutToHide, [this]()
        {
            allow_hide_ = true;

            if (leaved_)
                delayedHide();
        });

        connect(ui.button_ctrl_alt_del, &QPushButton::pressed,
                this, &DesktopPanel::onCtrlAltDelButton);

        connect(ui.action_alt_tab, &QAction::triggered,
                this, &DesktopPanel::onAltTabAction);

        connect(ui.action_alt_shift_tab, &QAction::triggered,
                this, &DesktopPanel::onAltShiftTabAction);

        connect(ui.action_printscreen, &QAction::triggered,
                this, &DesktopPanel::onPrintScreenAction);

        connect(ui.action_alt_printscreen, &QAction::triggered,
                this, &DesktopPanel::onAltPrintScreenAction);

        connect(ui.action_custom, &QAction::triggered,
                this, &DesktopPanel::onCustomAction);

        ui.button_send_keys->setMenu(keys_menu_);
    }
    else
    {
        Q_ASSERT(session_type == proto::auth::SESSION_TYPE_DESKTOP_VIEW);

        ui.button_send_keys->hide();
        ui.button_ctrl_alt_del->hide();
    }

    ui.frame->hide();
    adjustSize();

    hide_timer_id_ = startTimer(std::chrono::seconds(1));
}

void DesktopPanel::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == hide_timer_id_)
    {
        killTimer(hide_timer_id_);
        hide_timer_id_ = 0;

        ui.frame->setFixedWidth(ui.frame_buttons->width());

        ui.frame_buttons->hide();
        ui.frame->show();

        adjustSize();
        return;
    }

    QFrame::timerEvent(event);
}

void DesktopPanel::enterEvent(QEvent* event)
{
    leaved_ = false;

    if (allow_hide_)
    {
        if (hide_timer_id_)
        {
            killTimer(hide_timer_id_);
            hide_timer_id_ = 0;
        }

        ui.frame_buttons->show();
        ui.frame->hide();

        adjustSize();
    }

    QFrame::enterEvent(event);
}

void DesktopPanel::leaveEvent(QEvent* event)
{
    leaved_ = true;

    if (allow_hide_)
        delayedHide();

    QFrame::leaveEvent(event);
}

void DesktopPanel::onFullscreenButton(bool checked)
{
    if (checked)
    {
        ui.button_full_screen->setIcon(
            QIcon(QStringLiteral(":/icon/application-resize-actual.png")));
    }
    else
    {
        ui.button_full_screen->setIcon(
            QIcon(QStringLiteral(":/icon/application-resize-full.png")));
    }

    emit switchToFullscreen(checked);
}

void DesktopPanel::onAutosizeButton()
{
    if (ui.button_full_screen->isChecked())
    {
        ui.button_full_screen->setIcon(
            QIcon(QStringLiteral(":/icon/application-resize-full.png")));
        ui.button_full_screen->setChecked(false);
    }

    emit switchToAutosize();
}

void DesktopPanel::onCtrlAltDelButton()
{
    emit keySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_Delete);
}

void DesktopPanel::onAltTabAction()
{
    emit keySequence(Qt::AltModifier | Qt::Key_Tab);
}

void DesktopPanel::onAltShiftTabAction()
{
    emit keySequence(Qt::AltModifier | Qt::ShiftModifier | Qt::Key_Tab);
}

void DesktopPanel::onPrintScreenAction()
{
    emit keySequence(Qt::Key_Print);
}

void DesktopPanel::onAltPrintScreenAction()
{
    emit keySequence(Qt::AltModifier | Qt::Key_Print);
}

void DesktopPanel::onCustomAction()
{
    QKeySequence key_sequence = KeySequenceDialog::keySequence(this);

    for (int i = 0; i < key_sequence.count(); ++i)
        emit keySequence(key_sequence[i]);
}

void DesktopPanel::delayedHide()
{
    if (!ui.button_pin->isChecked() && !hide_timer_id_)
        hide_timer_id_ = startTimer(std::chrono::seconds(1));
}

} // namespace aspia
