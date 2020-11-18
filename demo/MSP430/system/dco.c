/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */
#include "dco.h"
/* ----------------------- Start implementation -----------------------------*/

void Init_Clock_16MHz()
{
    if (CALBC1_16MHZ == 0xFF)    // If calibration constant erased
    {
        while(1); // do not load, trap CPU!!
    }
    else                        // If 16MHz Calibration is present in Flash SegmentA Calibration Data
    {
        // Select lowest DCOx and MODx settings
        DCOCTL = 0;
        BCSCTL1 = CALBC1_16MHZ;     // 16MHz Range Selected from Calibration Data in Flash SegmentA
        DCOCTL = CALDCO_16MHZ;      // Set DCO step + modulation

        BCSCTL3 = MASK_VLOCLK;      // Set the Low Frequency clock to VLOCLK
    }
}
