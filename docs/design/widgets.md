# Drawing

Draw the contents of your `Widget`s from the various `draw*` methods.

If needed, call `invalidate` on a widget to cause it to redraw itself (async).

It's actually valid to draw directly on the framebuffer at any time, but an
`invalidate` may erase it.

# Refreshing

The normal flow is:

- invalidate widges as-needed
- widgets are added to set of things to be redrawn
- refresh fires periodically
- `FbScreen->update()` is called
- calls `draw` methods of all dirty widgets
- updates the framebuffer asynchronously

Call `invalidate()` when widget is out of date (eg button has been clicked).
Nothing special needs to be done after drawing; the refresh will handle updating
to the framebuffer.

Don't call `fb->update` or `fb->sync` directly.  This is automatically done by
the refresh.  Even `fb->sync` is rarely done.  Sync means waiting for the bits
to actually become visible.  This is a slow operation (tenths of a second).
However it might make sense to do a manual sync before sleeping.

`m_fb->update(nullptr)` is not the same as `FbScreen->update()`.  You almost
certainly want to update through the screen, since it redraws invalid widgets
first.
