/*  Copyright (C) 2005-2010, Axis Communications AB, LUND, SWEDEN
 *
 *  This file is part of RAPP.
 *
 *  RAPP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  You can use the comments under either the terms of the GNU Lesser General
 *  Public License version 3 as published by the Free Software Foundation,
 *  either version 3 of the License or (at your option) any later version, or
 *  the GNU Free Documentation License version 1.3 or any later version
 *  published by the Free Software Foundation; with no Invariant Sections, no
 *  Front-Cover Texts, and no Back-Cover Texts.
 *  A copy of the license is included in the documentation section entitled
 *  "GNU Free Documentation License".
 *
 *  RAPP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License and a copy of the GNU Free Documentation License along
 *  with RAPP. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file   rc_export.h
 *  @brief  RAPP Compute API exported symbol tags.
 */

#ifndef RC_EXPORT_H
#define RC_EXPORT_H


/*
 * -------------------------------------------------------------
 *  Constants
 * -------------------------------------------------------------
 */

/**
 *  Set the RC_EXPORT interface marker.
 *  Only export symbols if explicitly forced.
 */
#ifdef RAPP_FORCE_EXPORT
#ifdef _MSC_VER
#define RC_EXPORT __declspec(dllexport)

#elif __GNUC__ >= 4 || __GNUC__ == 3 && __GNUC_MINOR__ >= 3
#define RC_EXPORT __attribute__((visibility("default")))

#else /* Unsupported */
#define RC_EXPORT

#endif

#else
#define RC_EXPORT
#endif

#endif /* RC_EXPORT_H */
