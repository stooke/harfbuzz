/*
 * Copyright © 2018 Adobe Systems Incorporated.
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
 * Adobe Author(s): Michiharu Ariza
 */
#ifndef HB_CFF1_INTERP_CS_HH
#define HB_CFF1_INTERP_CS_HH

#include "hb.hh"
#include "hb-cff-interp-cs-common.hh"

namespace CFF {

using namespace OT;

struct CFF1CSInterpEnv : CSInterpEnv<Number, CFF1Subrs>
{
  template <typename ACC>
  inline void init (const ByteStr &str, ACC &acc, unsigned int fd)
  {
    SUPER::init (str, *acc.globalSubrs, *acc.privateDicts[fd].localSubrs);
    processed_width = false;
    has_width = false;
    transient_array.init ();
    transient_array.resize (kTransientArraySize);
    for (unsigned int i = 0; i < kTransientArraySize; i++)
      transient_array[i].set_int (0);
  }

  inline void fini (void)
  {
    transient_array.fini ();
    SUPER::fini ();
  }

  inline unsigned int check_width (void)
  {
    unsigned int arg_start = 0;
    if (!processed_width)
    {
      if ((SUPER::argStack.get_count () & 1) != 0)
      {
        width = SUPER::argStack[0];
        has_width = true;
        arg_start = 1;
      }
      processed_width = true;
    }
    return arg_start;
  }

  bool          processed_width;
  bool          has_width;
  Number        width;

  static const unsigned int kTransientArraySize = 32;
  hb_vector_t<Number, kTransientArraySize>  transient_array;

  private:
  typedef CSInterpEnv<Number, CFF1Subrs> SUPER;
};

template <typename OPSET, typename PARAM, typename PATH=PathProcsNull<CFF1CSInterpEnv, PARAM> >
struct CFF1CSOpSet : CSOpSet<Number, OPSET, CFF1CSInterpEnv, PARAM, PATH>
{
  static inline void process_op (OpCode op, CFF1CSInterpEnv &env, PARAM& param)
  {
    Number  n1, n2;

    switch (op) {

      case OpCode_and:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_int ((n1.to_real() != 0.0f) && (n2.to_real() != 0.0f));
        break;
      case OpCode_or:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_int ((n1.to_real() != 0.0f) || (n2.to_real() != 0.0f));
        break;
      case OpCode_not:
        n1 = env.argStack.pop_num ();
        env.argStack.push_int (n1.to_real() == 0.0f);
        break;
      case OpCode_abs:
        n1 = env.argStack.pop_num ();
        env.argStack.push_real (fabs(n1.to_real ()));
        break;
      case OpCode_add:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_real (n1.to_real() + n2.to_real());
        break;
      case OpCode_sub:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_real (n1.to_real() - n2.to_real());
        break;
      case OpCode_div:
        env.argStack.pop_num2 (n1, n2);
        if (unlikely (n2.to_real() == 0.0f))
          env.argStack.push_int (0);
        else
          env.argStack.push_real (n1.to_real() / n2.to_real());
        break;
      case OpCode_neg:
        n1 = env.argStack.pop_num ();
        env.argStack.push_real (-n1.to_real ());
        break;
      case OpCode_eq:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_int (n1.to_real() == n2.to_real());
        break;
      case OpCode_drop:
        n1 = env.argStack.pop_num ();
        break;
      case OpCode_put:
        env.argStack.pop_num2 (n1, n2);
        env.transient_array[n2.to_int ()] = n1;
        break;
      case OpCode_get:
        n1 = env.argStack.pop_num ();
        env.argStack.push (env.transient_array[n1.to_int ()]);
        break;
      case OpCode_ifelse:
        {
          env.argStack.pop_num2 (n1, n2);
          bool  test = n1.to_real () <= n2.to_real ();
          env.argStack.pop_num2 (n1, n2);
          env.argStack.push (test? n1: n2);
        }
        break;
      case OpCode_random:
        env.argStack.push_int (1);  /* we can't deal with random behavior; make it constant */
        break;
      case OpCode_mul:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push_real (n1.to_real() * n2.to_real());
        break;
      case OpCode_sqrt:
        n1 = env.argStack.pop_num ();
        env.argStack.push_real ((float)sqrt (n1.to_real ()));
        break;
      case OpCode_dup:
        n1 = env.argStack.pop_num ();
        env.argStack.push (n1);
        env.argStack.push (n1);
        break;
      case OpCode_exch:
        env.argStack.pop_num2 (n1, n2);
        env.argStack.push (n2);
        env.argStack.push (n1);
        break;
      case OpCode_index:
        {
          n1 = env.argStack.pop_num ();
          int i = n1.to_int ();
          if (i < 0) i = 0;
          if (unlikely ((unsigned int)i >= env.argStack.get_count ()))
          {
            env.set_error ();
            return;
          }
          env.argStack.push (env.argStack[env.argStack.get_count () - i - 1]);
        }
        break;
      case OpCode_roll:
        {
          env.argStack.pop_num2 (n1, n2);
          int n = n1.to_int ();
          int j = n2.to_int ();
          if (unlikely (n < 0 || (unsigned int)n > env.argStack.get_count ()))
          {
            env.set_error ();
            return;
          }
          if (likely (n > 0))
          {
            if (j < 0)
              j = n - (-j % n);
            j %= n;
            unsigned int top = env.argStack.get_count () - 1;
            unsigned int bot = top - n + 1;
            env.argStack.reverse_range (top - j + 1, top);
            env.argStack.reverse_range (bot, top - j);
            env.argStack.reverse_range (bot, top);
          }
        }
        break;
      default:
        SUPER::process_op (op, env, param);
        break;
    }
  }

  static inline void flush_args (CFF1CSInterpEnv &env, PARAM& param, unsigned int start_arg = 0)
  {
    start_arg = env.check_width ();
    SUPER::flush_args (env, param, start_arg);
  }

  private:
  typedef CSOpSet<Number, OPSET, CFF1CSInterpEnv, PARAM, PATH>  SUPER;
};

template <typename OPSET, typename PARAM>
struct CFF1CSInterpreter : CSInterpreter<CFF1CSInterpEnv, OPSET, PARAM> {};

} /* namespace CFF */

#endif /* HB_CFF1_INTERP_CS_HH */