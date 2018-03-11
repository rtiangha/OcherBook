/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

/** @mainpage
 *
 * OcherBook is an open-source ebook reader, supporting multiple file formats and
 * output devices.  The major goals are:
 *
 *     - Return control and privacy of the reading experience to the reader
 *     - Portable
 *     - Modular design
 *     - Small with few external dependencies.
 *
 * Perhaps the largest distinguishing implementation detail of OcherBook is that
 * its in-memory format is not DOM, and the rendering backends are not web
 * browsers.  Think DVI.  Therefore it is not going to render complex books nearly
 * as accurately, but it is far more portable and efficient.  This may eventually
 * turn out to be a mistake, but for now it's an advantage that I'm running with.
 *
 * @copyright OcherBook itself is GPLv3.  See COPYING.
 * See doc/LICENSES for all relevant licenses.
 */
