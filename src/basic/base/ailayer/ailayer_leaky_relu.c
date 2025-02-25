/**
 * \file basic/base/ailayer/ailayer_leaky_relu.c
 * \version 2.0alpha
 * \date 16.03.2021
 * \copyright  Copyright (C) 2020-2021  Fraunhofer Institute for Microelectronic Circuits and Systems.
    All rights reserved.

    AIfES is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \brief
 * \details
 */

#include "basic/base/ailayer/ailayer_leaky_relu.h"
#include "basic/base/aimath/aimath_basic.h"

const aicore_layertype_t ailayer_leaky_relu_type_s = {
#ifdef AIDEBUG_PRINT_MODULE_SPECS
    .name = "Leaky ReLU",
	.print_specs = ailayer_leaky_relu_print_specs
#else
    .name = 0,
    .print_specs = 0
#endif
};
const aicore_layertype_t *ailayer_leaky_relu_type = &ailayer_leaky_relu_type_s;

ailayer_t *ailayer_leaky_relu(ailayer_leaky_relu_t *layer,  ailayer_t *input_layer) ////const void *beta1,
{
    layer->base.layer_type = ailayer_leaky_relu_type;

	layer->base.input_layer = input_layer;
	input_layer->output_layer = &(layer->base);
	layer->base.layer_configuration = layer;
	layer->base.result.dtype = layer->dtype;
	layer->base.result.shape = input_layer->result.shape;
	layer->base.result.dim = input_layer->result.dim;

	layer->base.deltas.dtype = layer->dtype;
	layer->base.deltas.dim = 2;
	layer->base.deltas.shape = layer->base.result.shape;

	layer->base.forward = ailayer_leaky_relu_forward;
	layer->base.backward = ailayer_leaky_relu_backward;

	layer->base.calc_result_shape = ailayer_leaky_relu_calc_result_shape;
	layer->base.sizeof_paramem = 0;
	layer->base.set_paramem = 0;
	layer->base.sizeof_trainmem = 0;
	layer->base.set_trainmem = 0;

	layer->base.trainable_params_count = 0;

	return &(layer->base);
}

void ailayer_leaky_relu_forward(ailayer_t *self)
{
	ailayer_leaky_relu_t *layer = (ailayer_leaky_relu_t *)(self->layer_configuration);
	aitensor_t *x_in = &(self->input_layer->result);
	aitensor_t *x_out = &(self->result);

	layer->leaky_relu(x_in, layer->alpha, x_out);
	return;
}

void ailayer_leaky_relu_backward(ailayer_t *self)
{
	ailayer_leaky_relu_t *layer = (ailayer_leaky_relu_t *)(self->layer_configuration);
	aitensor_t *delta_in = &(self->deltas);
	aitensor_t *delta_out = &(self->output_layer->deltas);
	aitensor_t *x_in = &(self->input_layer->result);

	// delta_in = delta_out .* relu'(x_in)
	layer->d_leaky_relu(x_in, layer->alpha, delta_in);
	layer->multiply(delta_in, delta_out, delta_in);

	return;
}

void ailayer_leaky_relu_calc_result_shape(ailayer_t *self)
{
	/* Unused: Shape is already defined (Pointer)

	self->result.shape[0] = self->input_layer->result.shape[0];
	self->result.shape[1] = self->input_layer->result.shape[1];
	*/
	return;
}

#ifdef AIDEBUG_PRINT_MODULE_SPECS
void ailayer_leaky_relu_print_specs(const ailayer_t *self, int (*print)(const char *format, ...))
{
    ailayer_leaky_relu_t *self_casted = (ailayer_leaky_relu_t *) self;

    print("alpha: ");
    self_casted->dtype->print_aiscalar(self_casted->alpha, print);
    return;
}
#endif

