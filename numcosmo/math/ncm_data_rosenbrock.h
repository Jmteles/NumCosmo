/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/***************************************************************************
 *            ncm_data_rosenbrock.h
 *
 *  Sat April 17 11:11:28 2021
 *  Copyright  2021  Sandro Dias Pinto Vitenti
 *  <sandro@isoftware.com.br>
 ****************************************************************************/
/*
 * ncm_data_rosenbrock.h
 * Copyright (C) 2021 Sandro Dias Pinto Vitenti <sandro@isoftware.com.br>
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

#ifndef _NCM_DATA_ROSENBROCK_H_
#define _NCM_DATA_ROSENBROCK_H_

#include <glib.h>
#include <glib-object.h>
#include <numcosmo/build_cfg.h>
#include <numcosmo/math/ncm_data_gauss_cov.h>

G_BEGIN_DECLS

#define NCM_TYPE_DATA_ROSENBROCK             (ncm_data_rosenbrock_get_type ())
#define NCM_DATA_ROSENBROCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), NCM_TYPE_DATA_ROSENBROCK, NcmDataRosenbrock))
#define NCM_DATA_ROSENBROCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), NCM_TYPE_DATA_ROSENBROCK, NcmDataRosenbrockClass))
#define NCM_IS_DATA_ROSENBROCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NCM_TYPE_DATA_ROSENBROCK))
#define NCM_IS_DATA_ROSENBROCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), NCM_TYPE_DATA_ROSENBROCK))
#define NCM_DATA_ROSENBROCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), NCM_TYPE_DATA_ROSENBROCK, NcmDataRosenbrockClass))

typedef struct _NcmDataRosenbrockClass NcmDataRosenbrockClass;
typedef struct _NcmDataRosenbrock NcmDataRosenbrock;
typedef struct _NcmDataRosenbrockPrivate NcmDataRosenbrockPrivate;

struct _NcmDataRosenbrockClass
{
  /*< private >*/
  NcmDataClass parent_class;
};

struct _NcmDataRosenbrock
{
  /*< private >*/
  NcmData parent_instance;
  NcmDataRosenbrockPrivate *priv;
};

GType ncm_data_rosenbrock_get_type (void) G_GNUC_CONST;

NcmDataRosenbrock *ncm_data_rosenbrock_new (void);
NcmDataRosenbrock *ncm_data_rosenbrock_ref (NcmDataRosenbrock *drb);
void ncm_data_rosenbrock_free (NcmDataRosenbrock *drb);
void ncm_data_rosenbrock_clear (NcmDataRosenbrock **drb);

G_END_DECLS

#endif /* _NCM_DATA_ROSENBROCK_H_ */
