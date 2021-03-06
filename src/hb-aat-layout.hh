/*
 * Copyright © 2017  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef HB_AAT_LAYOUT_HH
#define HB_AAT_LAYOUT_HH

#include "hb.hh"

#include "hb-ot-shape.hh"


struct hb_aat_feature_mapping_t
{
  hb_tag_t otFeatureTag;
  uint16_t aatFeatureType;
  uint16_t selectorToEnable;
  uint16_t selectorToDisable;

  static inline int cmp (const void *key_, const void *entry_)
  {
    hb_tag_t key = * (unsigned int *) key_;
    const hb_aat_feature_mapping_t * entry = (const hb_aat_feature_mapping_t *) entry_;
    return key < entry->otFeatureTag ? -1 :
	   key > entry->otFeatureTag ? 1 :
	   0;
  }
};

HB_INTERNAL const hb_aat_feature_mapping_t *
hb_aat_layout_find_feature_mapping (hb_tag_t tag);

HB_INTERNAL void
hb_aat_layout_compile_map (const hb_aat_map_builder_t *mapper,
			   hb_aat_map_t *map);

HB_INTERNAL hb_bool_t
hb_aat_layout_has_substitution (hb_face_t *face);

HB_INTERNAL void
hb_aat_layout_substitute (hb_ot_shape_plan_t *plan,
			  hb_font_t *font,
			  hb_buffer_t *buffer);

HB_INTERNAL hb_bool_t
hb_aat_layout_has_positioning (hb_face_t *face);

HB_INTERNAL void
hb_aat_layout_position (hb_ot_shape_plan_t *plan,
			hb_font_t *font,
			hb_buffer_t *buffer);

HB_INTERNAL hb_bool_t
hb_aat_layout_has_tracking (hb_face_t *face);

HB_INTERNAL void
hb_aat_layout_track (hb_ot_shape_plan_t *plan,
		     hb_font_t *font,
		     hb_buffer_t *buffer);

HB_INTERNAL hb_language_t
_hb_aat_language_get (hb_face_t *face,
		      unsigned int i);


#endif /* HB_AAT_LAYOUT_HH */
