/*
 * Copyright (C) 2012, Paul Evans <leonerd@leonerd.org.uk>
 * Copyright (C) 2014, Olli Vanhoja <olli.vanhoja@cs.helsinki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <gd.h>

/* We can't use the one defined in Xmd.h because that's an "unsigned int",
 * which comes out as a 32bit type always. We need this to be 64bit on 64bit
 * machines.
 */
typedef unsigned long int CARD32;

/* Note:
 *  dispite the fact this routine specifically loads 32bit data, it needs to
 *  load it into an unsigned long int array, not a guint32 array. The
 *  XChangeProperty() call wants to see a native size array when format == 32,
 *  not necessarily a 32bit one.
 */

int load_icon(gchar *filename, int *ndata, CARD32 **data)
{
    FILE *iconfile = fopen(filename, "r");
    int width, height, x, y, i;
    gdImagePtr icon;

    if (!iconfile) {
        fprintf(stderr, "fopen(%s)", filename);
        return 1;
    }

    icon = gdImageCreateFromPng(iconfile);

    fclose(iconfile);

    width = gdImageSX(icon);
    height = gdImageSY(icon);

    (*ndata) = (width * height) + 2;

    (*data) = g_new0(CARD32, (*ndata));

    i = 0;
    (*data)[i++] = width;
    (*data)[i++] = height;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int pixcolour, alpha;
            guint8 *cols;

            // data is RGBA
            // We'll do some horrible data-munging here
            cols = (guint8*)&((*data)[i++]);

            pixcolour = gdImageGetPixel(icon, x, y);

            cols[0] = gdImageBlue(icon, pixcolour);
            cols[1] = gdImageGreen(icon, pixcolour);
            cols[2] = gdImageRed(icon, pixcolour);

            /* Alpha is more difficult */
            alpha = 127 - gdImageAlpha(icon, pixcolour); // 0 to 127

            // Scale it up to 0 to 255; remembering that 2*127 should be max
            if (alpha == 127)
                alpha = 255;
            else
                alpha *= 2;

            cols[3] = alpha;
        }
    }

    gdImageDestroy(icon);
    return 0;
}

int set_icon(char *filename, Display *display, Window *window)
{
    Atom property;
    int nelements;
    unsigned char *data;
    int result;

    property = XInternAtom(display, "_NET_WM_ICON", 0);
    if (!property) {
        fprintf(stderr, "XInternAtom(property)");
        return 1;
    }

    result = load_icon(filename, &nelements, (CARD32 **)&data);
    if (result) {
        return 1;
    }

    result = XChangeProperty(display, *window, property, XA_CARDINAL, 32, PropModeReplace,
                             data, (guint)nelements);
    if (result) {
        fprintf(stderr, "XInternAtom(property)");
        return 1;
    }

    return 0;
}
