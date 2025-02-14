/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/***************************************************************************
 *            nc_de_cont.c
 *
 *  Thu December 15 15:08:26 2020
 *  Copyright  2018  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * nc_de_cont.c
 * Copyright (C) 2020 Sandro Dias Pinto Vitenti <sandro@isoftware.com.br>
 *
 * numcosmo is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * numcosmo is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:nc_de_cont
 * @title: NcDECont
 * @short_description: Dark energy contraction perturbations model
 *
 * FIXME
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */
#include "build_cfg.h"

#include "nc_de_cont.h"

#ifndef NUMCOSMO_GIR_SCAN
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_sf_hyperg.h>


#ifdef HAVE_ACB_H
#include <acb.h>
#include <acb_hypgeom.h>
#endif /* HAVE_ACB_H  */

#include <nvector/nvector_serial.h>

#endif /* NUMCOSMO_GIR_SCAN */

struct _NcDEContPrivate
{
  gdouble Omega_w;
  gdouble Omega_L;
  gdouble cs2;
  gdouble cs;
  gdouble w;
 };

enum
{
  PROP_0,
  PROP_OMEGA_W,
  PROP_OMEGA_L,
  PROP_CS2,
  PROP_W,
};

G_DEFINE_TYPE_WITH_PRIVATE (NcDECont, nc_de_cont, NCM_TYPE_CSQ1D);

static void
nc_de_cont_init (NcDECont *dec)
{
  NcDEContPrivate * const self = dec->priv = nc_de_cont_get_instance_private (dec);

  self->Omega_w = 0.0;
  self->Omega_L = 0.0;
  self->cs2     = 0.0;
  self->cs      = 0.0;
  self->w       = 0.0;
}

static void
_nc_de_cont_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  NcDECont *dec = NC_DE_CONT (object);
  NcDEContPrivate * const self = dec->priv;
  g_return_if_fail (NC_IS_DE_CONT (object));

  switch (prop_id)
  {
    case PROP_OMEGA_W:
      self->Omega_w = g_value_get_double (value);
      break;
    case PROP_OMEGA_L:
      self->Omega_L = g_value_get_double (value);
      break;
    case PROP_CS2:
      self->cs2 = g_value_get_double (value);
      self->cs  = sqrt (self->cs2);
      break;
    case PROP_W:
      self->w = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
_nc_de_cont_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  NcDECont *dec = NC_DE_CONT (object);
  NcDEContPrivate * const self = dec->priv;
  g_return_if_fail (NC_IS_DE_CONT (object));

  switch (prop_id)
  {
    case PROP_OMEGA_W:
      g_value_set_double (value, self->Omega_w);
      break;
    case PROP_OMEGA_L:
      g_value_set_double (value, self->Omega_L);
      break;
    case PROP_CS2:
      g_value_set_double (value, self->cs2);
      break;
    case PROP_W:
      g_value_set_double (value, self->w);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
_nc_de_cont_dispose (GObject *object)
{
  /*NcDECont *dec = NC_DE_CONT (object);*/
  /*NcDEContPrivate * const self = dec->priv;*/

  /* Chain up : end */
  G_OBJECT_CLASS (nc_de_cont_parent_class)->dispose (object);
}

static void
_nc_de_cont_finalize (GObject *object)
{
  /*NcDECont *dec = NC_DE_CONT (object);*/
  /*NcDEContPrivate * const self = dec->priv;*/
  
  /* Chain up : end */
  G_OBJECT_CLASS (nc_de_cont_parent_class)->finalize (object);
}

static gdouble _nc_de_cont_eval_m          (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_int_1_m    (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_int_mnu2   (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_int_qmnu2  (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_int_q2mnu2 (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_nu         (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_xi         (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_F1         (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);
static gdouble _nc_de_cont_eval_F2         (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k);

static void
nc_de_cont_class_init (NcDEContClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  NcmCSQ1DClass *csq1d_class = NCM_CSQ1D_CLASS (klass);

  object_class->set_property = &_nc_de_cont_set_property;
  object_class->get_property = &_nc_de_cont_get_property;
  object_class->dispose      = &_nc_de_cont_dispose;
  object_class->finalize     = &_nc_de_cont_finalize;
  
  g_object_class_install_property (object_class,
                                   PROP_OMEGA_W,
                                   g_param_spec_double ("Omegaw",
                                                        NULL,
                                                        "\\Omega_w",
                                                        1.0e-5, G_MAXDOUBLE, 1.0,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
  g_object_class_install_property (object_class,
                                   PROP_OMEGA_L,
                                   g_param_spec_double ("OmegaL",
                                                        NULL,
                                                        "\\Omega_\\Lambda",
                                                        0.0, G_MAXDOUBLE, 1.0,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
  g_object_class_install_property (object_class,
                                   PROP_CS2,
                                   g_param_spec_double ("cs2",
                                                        NULL,
                                                        "c_s^2",
                                                        1.0e-30, 1.0, 1.0e-2,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
  g_object_class_install_property (object_class,
                                   PROP_W,
                                   g_param_spec_double ("w",
                                                        NULL,
                                                        "w",
                                                        1.0e-30, 1.0, 1.0e-2,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_NAME | G_PARAM_STATIC_BLURB));
  csq1d_class->eval_m          = &_nc_de_cont_eval_m;
  csq1d_class->eval_int_1_m    = &_nc_de_cont_eval_int_1_m;
  csq1d_class->eval_int_mnu2   = &_nc_de_cont_eval_int_mnu2;
  csq1d_class->eval_int_qmnu2  = &_nc_de_cont_eval_int_qmnu2;
  csq1d_class->eval_int_q2mnu2 = &_nc_de_cont_eval_int_q2mnu2;
  csq1d_class->eval_nu         = &_nc_de_cont_eval_nu;
  csq1d_class->eval_xi         = &_nc_de_cont_eval_xi;
  csq1d_class->eval_F1         = &_nc_de_cont_eval_F1;
  csq1d_class->eval_F2         = &_nc_de_cont_eval_F2;
}

static gdouble
_nc_de_cont_eval_m (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_1p3w = pow (t, 1.0 + 3.0 * self->w);
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2     = self->Omega_w * t_3p3w + self->Omega_L;
  const gdouble mE     = sqrt (E2);

  return 3.0 * (1.0 + self->w) * t_1p3w * self->Omega_w / (2.0 * self->cs2 * mE);
}

static gdouble
_nc_de_cont_eval_int_1_m (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_3w   = pow (t, 3.0 * self->w);
  const gdouble t_1p3w = t * t_3w;
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2mOL  = self->Omega_w * t_3p3w;
  const gdouble E2     = E2mOL + self->Omega_L;
  const gdouble mE     = sqrt (E2);
  const gdouble _2F1   = gsl_sf_hyperg_2F1 (-0.5, 1.0, 1.0 / (1.0 + self->w), E2mOL / E2);

  return -2.0 * self->cs2 * mE * _2F1 / (9.0 * self->w * (1.0 + self->w) * self->Omega_w * t_3w);
}

static gdouble
_nc_de_cont_eval_int_mnu2 (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble three1pw = 3.0 * (1.0 + self->w);
  const gdouble t_3p3w   = pow (t, three1pw);
  const gdouble z        = - t_3p3w * self->Omega_w / self->Omega_L;
  const gdouble z2       = z * z;
  const gdouble z3       = z2 * z;
  const gdouble z4       = z2 * z2;
  const gdouble T0       = three1pw - 1.0;
  const gdouble T1       = T0 + three1pw;
  const gdouble T2       = T1 + three1pw;
  const gdouble T3       = T2 + three1pw;
  const gdouble T4       = T3 + three1pw;
  const gdouble prefac   = k * k *  three1pw * self->Omega_w * pow (t, T0) / (2.0 * T0 * sqrt (gsl_pow_3 (self->Omega_L)));

  /*err[0] = fabs (315.0 / 128.0 * T0 * (z4 / T4));*/

  return prefac * (1.0 + T0 * ((3.0 / 2.0) * z / T1 + (15.0 / 8.0) * z2 / T2 + (35.0 / 16.0) * z3 / T3 + (315.0 / 128.0) * z4 / T4));
}

static gdouble
_nc_de_cont_eval_int_qmnu2 (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble three1pw = 3.0 * (1.0 + self->w);
  const gdouble t_3p3w   = pow (t, three1pw);
  const gdouble z        = - t_3p3w * self->Omega_w / self->Omega_L;
  const gdouble z2       = z * z;
  const gdouble z3       = z2 * z;
  const gdouble z4       = z2 * z2;
  const gdouble prefac   = - self->cs2 * gsl_pow_2 (k * t) / (6.0 * self->w * self->Omega_L);
  const gdouble n1       = (3.0 + self->w);
  const gdouble n2       = n1 * (5.0 + 3.0 * self->w);
  const gdouble n3       = n2 * (7.0 + 5.0 * self->w);
  const gdouble n4       = n3 * (9.0 + 7.0 * self->w);
  const gdouble d1       = (5.0 + 3.0 * self->w);
  const gdouble d2       =  4.0 * (2.0 + self->w) * (4.0 + 3.0 * self->w);
  const gdouble d3       =  4.0 * (2.0 + self->w) * (3.0 + 2.0 * self->w) * (11.0 + 9.0 * self->w);
  const gdouble d4       = d2 * 4.0 * (3.0 + 2.0 * self->w) * (7.0 + 6.0 * self->w);

  /*err[0] = n4 * z4 / d4;*/

  return prefac * (1.0 + n1 * z / d1 + n2 * z2 / d2 + n3 * z3 / d3 + n4 * z4 / d4);
}

static gdouble
_nc_de_cont_eval_int_q2mnu2 (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble three1pw = 3.0 * (1.0 + self->w);
  const gdouble t_3p3w   = pow (t, three1pw);
  const gdouble z        = - t_3p3w * self->Omega_w / self->Omega_L;
  const gdouble z2       = z * z;
  const gdouble z3       = z2 * z;
  const gdouble z4       = z2 * z2;
  const gdouble w2       = self->w * self->w;
  const gdouble w3       = w2 * self->w;
  const gdouble w4       = w2 * w2;
  const gdouble w5       = w2 * w3;
  const gdouble w6       = w3 * w3;
  const gdouble T0       = 2.0 - 3.0 * self->w;
  const gdouble prefac   = 2.0 * gsl_pow_2 (self->cs2 * k) * pow (t, T0) / (27.0 * w2 * sqrt (self->Omega_L) * self->Omega_w * (1.0 + self->w) * T0);
  const gdouble n1       = 3.0 + 2.0 * self->w;
  const gdouble n2       =    30.0 +    41.0 * self->w +    16.0 * w2 +     2.0 * w3;
  const gdouble n3       =   210.0 +   447.0 * self->w +   336.0 * w2 +   106.0 * w3 +    12.0 * w4;
  const gdouble n4       = 15120.0 + 51924.0 * self->w + 72480.0 * w2 + 52591.0 * w3 + 20898.0 * w4 + 4308.0 * w5 + 360.0 * w6;
  const gdouble d1       = 10.0;
  const gdouble d2       =   8.0 * (2.0 + self->w) * (8.0 + 3.0 * self->w);
  const gdouble d3       =  16.0 * (2.0 + self->w) * (3.0 + 2.0 * self->w) * (11.0 + 6.0 * self->w);
  const gdouble d4       = 128.0 * gsl_pow_2 (2.0 + self->w) * (3.0 + 2.0 * self->w) * (4.0 + 3.0 * self->w) * (14.0 + 9.0 * self->w);

  /*err[0] = fabs (T0 * n4 * z4 / d4);*/

  return prefac * (1.0 + T0 * (n1 * z / d1 + n2 * z2 / d2 + n3 * z3 / d3 + n4 * z4 / d4));
}


static gdouble
_nc_de_cont_eval_nu  (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_1p3w = pow (t, 1.0 + 3.0 * self->w);
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2     = self->Omega_w * t_3p3w + self->Omega_L;

  return self->cs * k / sqrt (E2);
}

static gdouble
_nc_de_cont_eval_xi  (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_1p3w = pow (t, 1.0 + 3.0 * self->w);
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2     = self->Omega_w * t_3p3w + self->Omega_L;

  return log (3.0 * k * (1.0 + self->w) * t_1p3w * self->Omega_w / (2.0 * self->cs * E2));

}

static gdouble
_nc_de_cont_eval_F1  (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_1p3w = pow (t, 1.0 + 3.0 * self->w);
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2     = self->Omega_w * t_3p3w + self->Omega_L;
  const gdouble mE     = sqrt (E2);

  return (3.0 * (1.0 + self->w) * self->Omega_L - 2.0 * E2) / (2.0 * self->cs * mE * k * t);
}

static gdouble
_nc_de_cont_eval_F2  (NcmCSQ1D *csq1d, NcmModel *model, const gdouble t, const gdouble k)
{
  NcDECont *dec = NC_DE_CONT (csq1d);
  NcDEContPrivate * const self = dec->priv;

  const gdouble t_1p3w = pow (t, 1.0 + 3.0 * self->w);
  const gdouble t_3p3w = t * t * t_1p3w;
  const gdouble E2     = self->Omega_w * t_3p3w + self->Omega_L;

  return -(gsl_pow_2 (3.0 * (1.0 + self->w)) * self->Omega_L * self->Omega_w * t_3p3w + 2.0 * E2 * E2 * (1.0 + 3.0 * self->w)) / (8.0 * self->cs2 * E2 * gsl_pow_2 (k * t));
}

/**
 * nc_de_cont_new:
 * @Omegaw: the value of $\Omega_w$
 * @OmegaL: the value of $\Omega_\Lambda$
 * @cs2: the value of $c_s^2$
 * @w: the value of $w$
 * 
 * Creates a new #NcDECont object.
 * 
 * Returns: (transfer full): a new #NcDECont.
 */
NcDECont *
nc_de_cont_new (const gdouble Omegaw, const gdouble OmegaL, const gdouble cs2, const gdouble w)
{
  NcDECont *dec = g_object_new (NC_TYPE_DE_CONT,
                                "Omegaw", Omegaw,
                                "OmegaL", OmegaL,
                                "cs2",    cs2,
                                "w",      w,
                                NULL);
  return dec;
}

/**
 * nc_de_cont_ref:
 * @dec: a #NcDECont
 *
 * Increase the reference of @dec by one.
 *
 * Returns: (transfer full): @dec.
 */
NcDECont *
nc_de_cont_ref (NcDECont *dec)
{
  return g_object_ref (dec);
}

/**
 * nc_de_cont_free:
 * @dec: a #NcDECont
 *
 * Decrease the reference count of @dec by one.
 *
 */
void
nc_de_cont_free (NcDECont *dec)
{
  g_object_unref (dec);
}

/**
 * nc_de_cont_clear:
 * @dec: a #NcDECont
 *
 * Decrease the reference count of @dec by one, and sets the pointer *dec to
 * NULL.
 *
 */
void
nc_de_cont_clear (NcDECont **dec)
{
  g_clear_object (dec);
}

