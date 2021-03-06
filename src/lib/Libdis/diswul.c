/*
 * Copyright (C) 1994-2018 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * PBS Pro is free software. You can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PBS Pro is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * For a copy of the commercial license terms and conditions,
 * go to: (http://www.pbspro.com/UserArea/agreement.html)
 * or contact the Altair Legal Department.
 *
 * Altair’s dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of PBS Pro and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair’s trademarks, including but not limited to "PBS™",
 * "PBS Professional®", and "PBS Pro™" and Altair’s logos is subject to Altair's
 * trademark licensing policies.
 *
 */
/**
 * @file	diswul.c
 *
 * @par Synopsis:
 *	int diswul(int stream, unsigned long value)
 *
 *	Converts <value> into a Data-is-Strings unsigned integer and sends it to
 *	<stream>.
 *
 *	This format for character strings representing unsigned integers can
 *	best be understood through the decoding algorithm:
 *
 *	1. Initialize the digit count to 1.
 *
 *	2. Read the next character; if it is a plus sign, go to step (4); if it
 *	   is a minus sign, post an error.
 *
 *	3. Decode a new count from the digit decoded in step (2) and the next
 *	   count - 1 digits; repeat step (2).
 *
 *	4. Decode the next count digits as the unsigned integer.
 *
 *	Returns DIS_SUCCESS if everything works well.  Returns an error code
 *	otherwise.  In case of an error, no characters are sent to <stream>.
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "dis.h"
#include "dis_.h"

/**
 * @brief
 *      Converts <value> into a Data-is-Strings unsigned integer and sends
 *      it to <stream>.
 *
 * @param[in] stream    socket fd
 * @param[in] value     value to be converted
 *
 * @return      int
 * @retval      DIS_SUCCESS     success
 * @retval      error code      error
 *
 */
int
diswul(int stream, unsigned long value)
{
	int		retval;
	unsigned	ndigs;
	char		*cp;

	assert(stream >= 0);
	assert(dis_puts != NULL);
	assert(disw_commit != NULL);

	cp = discul_(&dis_buffer[DIS_BUFSIZ], value, &ndigs);
	*--cp = '+';
	while (ndigs > 1)
		cp = discui_(cp, ndigs, &ndigs);
	retval = (*dis_puts)(stream, cp,
		(size_t)(&dis_buffer[DIS_BUFSIZ] - cp)) < 0 ?
		DIS_PROTO : DIS_SUCCESS;
	return (((*disw_commit)(stream, retval == DIS_SUCCESS) < 0) ?
		DIS_NOCOMMIT : retval);
}
