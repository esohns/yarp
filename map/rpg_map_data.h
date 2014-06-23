/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RPG_MAP_DATA_H
#define RPG_MAP_DATA_H

/* Circles ==================================================================*/

/*
 * These numbers are limit offsets for one quadrant of a circle of a given
 * radius (the first number of each line) from the source.  The number in
 * the comment is the element number (so pointers can be set up).  Each
 * "circle" has as many elements as its radius+1.  The radius is the number
 * of points away from the source that the limit exists.  The radius of the
 * offset on the same row as the source *is* included so we don't have to
 * make an extra check.  For example, a circle of radius 4 has offsets:
 *
 *				XXX	+2
 *				...X	+3
 *				....X	+4
 *				....X	+4
 *				@...X   +4
 *
 */
static unsigned char RPG_Map_CircleData[] = {
/*  0*/	 1, 1,
/*  2*/	 2, 2, 1,
/*  5*/	 3, 3, 2, 1,
/*  9*/	 4, 4, 4, 3, 2,
/* 14*/	 5, 5, 5, 4, 3, 2,
/* 20*/	 6, 6, 6, 5, 5, 4, 2,
/* 27*/	 7, 7, 7, 6, 6, 5, 4, 2,
/* 35*/	 8, 8, 8, 7, 7, 6, 6, 4, 2,
/* 44*/	 9, 9, 9, 9, 8, 8, 7, 6, 5, 3,
/* 54*/	10,10,10,10, 9, 9, 8, 7, 6, 5, 3,
/* 65*/	11,11,11,11,10,10, 9, 9, 8, 7, 5, 3,
/* 77*/	12,12,12,12,11,11,10,10, 9, 8, 7, 5, 3,
/* 90*/	13,13,13,13,12,12,12,11,10,10, 9, 7, 6, 3,
/*104*/	14,14,14,14,13,13,13,12,12,11,10, 9, 8, 6, 3,
/*119*/	15,15,15,15,14,14,14,13,13,12,11,10, 9, 8, 6, 3,
/*135*/ 16 /* should be MAX_RADIUS+1; used to terminate range loops -dlc */
};

/*
 * These are the starting indexes into the circle_data[] array for a
 * circle of a given radius.
 */
static unsigned char RPG_Map_CircleStart[] = {
/*  */	  0,	/* circles of radius zero are not used */
/* 1*/    0,
/* 2*/	  2,
/* 3*/	  5,
/* 4*/	  9,
/* 5*/	 14,
/* 6*/	 20,
/* 7*/	 27,
/* 8*/	 35,
/* 9*/	 44,
/*10*/	 54,
/*11*/	 65,
/*12*/	 77,
/*13*/	 90,
/*14*/	104,
/*15*/	119,
};

#endif
