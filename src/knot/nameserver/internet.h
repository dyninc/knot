/*!
 * \file internet.h
 *
 * \author Marek Vavrusa <marek.vavrusa@nic.cz>
 *
 * \brief IN zone lookup.
 *
 * \addtogroup query_processing
 * @{
 */
/*  Copyright (C) 2013 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _KNOT_INTERNET_H_
#define _KNOT_INTERNET_H_

#include "libknot/packet/pkt.h"
#include "knot/zone/zonedb.h"

/* Query data (from query processing). */
struct query_data;

/*!
 * \brief Answer query from IN class zone.
 *
 * \retval FAIL if it encountered an error.
 * \retval DONE if finished.
 */
int internet_answer(knot_pkt_t *resp, struct query_data *qdata);

/*! \brief Require given QUERY TYPE or return error code. */
#define NS_NEED_QTYPE(qdata, qtype_want, error_rcode) \
	if (knot_pkt_qtype((qdata)->query) != (qtype_want)) { \
		qdata->rcode = (error_rcode); \
		return NS_PROC_FAIL; \
	}

/*! \brief Require given QUERY NAME or return error code. */
#define NS_NEED_QNAME(qdata, qname_want, error_rcode) \
	if (!knot_dname_is_equal(knot_pkt_qname((qdata)->query), (qname_want))) { \
		qdata->rcode = (error_rcode); \
		return NS_PROC_FAIL; \
	}

/*! \brief Require existing zone or return failure. */
#define NS_NEED_ZONE(qdata, error_rcode) \
	if ((qdata)->zone == NULL) { \
		qdata->rcode = (error_rcode); \
		return NS_PROC_FAIL; \
	}

/*! \brief Require existing zone contents or return failure. */
#define NS_NEED_ZONE_CONTENTS(qdata, error_rcode) \
	if ((qdata)->zone->contents == NULL) { \
		qdata->rcode = (error_rcode); \
		return NS_PROC_FAIL; \
	}

/*! \brief Require authentication. */
#define NS_NEED_AUTH(acl, qdata) \
	if (!process_query_acl_check((acl), (qdata))) { \
		return NS_PROC_FAIL; \
	} else { \
		if (process_query_verify(qdata) != KNOT_EOK) { \
			return NS_PROC_FAIL; \
		} \
	}

#endif /* _KNOT_INTERNET_H_ */

/*! @} */