/**
 * \file basic/base/ailayer/ailayer_leaky_relu.h
 * \internal
 * \date 15.03.2021
 * \endinternal
 * \version 2.0alpha
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
 * \brief Base \link ailayer layer \endlink implementation of the Leaky ReLU activation layer
 *
 * This is an "abstract" data-type independent implementation. To use the layer use one of the provided
 * implementations for a specific hardware and data-type (for example from ailayer_leaky_relu_default.h) or set
 * the required math functions on your own.
 *
 * \image html ailayer_leaky_relu_schematic.png width=200px
 *
 * The Leaky ReLU layer is used as an activation function layer right after a dense layer. It calculates
  * @f[
  *  y = \begin{cases}
            \alpha x & \text{if } x < 0\\
            x & \text{if } x \geq 0
        \end{cases}
  * @f]
 * for every element of the input tensor.
 *
 * The results of the forward pass of this layer are written to the result tensor of the base ailayer_t struct.
 */

#ifndef LEAKY_RELU_LAYER
#define LEAKY_RELU_LAYER

#include "core/aifes_core.h"

typedef struct ailayer_leaky_relu 	ailayer_leaky_relu_t;

/** @brief General \link ailayer_leaky_relu.h Leaky ReLU layer \endlink struct
*
*/
struct ailayer_leaky_relu {
    ailayer_t base; /**< Inherited field members from general ailayer struct. */
	const aimath_dtype_t *dtype; /**< Data type of the input and inference result values. */

	/** @name Layer configuration
	 * @brief Required configuration parameters for the layer
	 *
	 * These fields have to be configured by the user before calling the initializer function.
	 */
	///@{
	void *alpha; /**< Parameter \f$ \alpha \f$ used to calculate Leaky ReLU function for input values < 0. */
	///@}

	/** @name Math functions
	 * @brief Required data type specific math functions
	 */
	///@{

    /** @brief Required math function: Leaky ReLU
	 *
	 * Requires a math function that calculates the element wise Leaky ReLU of a tensor:\n
     * @f[
     *  result_{i} = \begin{cases}
            \alpha \cdot x_i & \text{if } x_i < 0 \\
            x_i & \text{if } x_i \geq 0
        \end{cases}
     * @f]
     *
     * @param x         N-dimensional tensor (input)
     * @param result    N-dimensional tensor (output)
	 */
	void (*leaky_relu)(const aitensor_t *x, const void *alpha, aitensor_t *result);

	/** @brief Required math function: Derivative of Leaky ReLU
	 *
	 * Requires a math function that calculates the element wise Leaky ReLU derivative of a tensor:\n
     * @f[
     *  result_{i} = \begin{cases}
                    \alpha & \text{if } x_i < 0\\
                    1 & \text{if } x_i \geq 0
                    \end{cases}
     * @f]
     *
     * @param x         N-dimensional tensor (input)
     * @param result    N-dimensional tensor (output)
	 */
	void (*d_leaky_relu)(const aitensor_t *x, const void *alpha, aitensor_t *result);

	/** @brief Required math function: Element wise tensor multiplication
	 *
	 * Requires a math function that multiplies two tensors element wise:\n
     * @f[
     *  result = a \circ b
     * @f]
	 */
	void (*multiply)(const aitensor_t *a, const aitensor_t *b, aitensor_t *result);

    ///@}
};

/** @brief Leaky ReLU layer type
 *
 * Defines the type of the layer (for example for type checks and debug prints).
 * See aicore_layertype for more information about the layer type.
 */
extern const aicore_layertype_t *ailayer_leaky_relu_type;

/** @brief Initialize and connect the given Leaky ReLU layer
 *
 * This function represents the "constructor" of the abstract Leaky ReLU layer. It initializes the layer structure
 * and connects it to the previous layer.\n
 * This function is not intended to call it directly. Instead use one of the data type specific implementations
 * (like for example ailayer_leaky_relu_f32_default()).
 *
 * @param *layer        The layer to initialize.
 * @param *input_layer  The previous layer that provides the inputs to the layer.
 * @return  Pointer to the (successfully) initialized general layer structure (ailayer_leaky_relu.base).
 */
ailayer_t *ailayer_leaky_relu(ailayer_leaky_relu_t *layer, ailayer_t *input_layer);

/** @brief Calculate the forward pass for given Leaky ReLU layer
 *
 * *Implementation of ailayer.forward.*
 *
 * It uses the result tensor of the previous layer as input and writes the result of the forward pass
 * to the result tensor (ailayer.result) of the given layer.
 *
 * Calculation of the forward pass result:
 * @f[
 *  x_{out} \leftarrow LeakyReLU(x_{in})
 * @f]
 *
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ x_{out} \f$:	 Result of the forward pass of this layer\n\n
 *
 * Used math functions:
 * * ailayer_leaky_relu.leaky_relu
 *
 * @param *self Layer to calculate the forward path for.
 */
void ailayer_leaky_relu_forward(ailayer_t *self);

/** @brief Calculate the backward pass for the given Leaky ReLU layer
 *
 * *Implementation of ailayer.backward.*
 *
 * It uses the deltas tensor of the next layer as input and writes the result of the backward pass
 * to the deltas tensor (ailayer.deltas) of the given layer.
 *
 * Calculation of the errors for the previous layer:
 * @f[
 *  \delta_{in} \leftarrow \delta_{out} \circ LeakyReLU'(x_{in})
 * @f]
 *
 * \f$ x_{in} \f$:	 Result of the forward pass of the previous layer\n
 * \f$ \delta_{in} \f$:	 Result of the backward pass of this layer\n
 * \f$ \delta_{out} \f$:	 Result of the backward pass of the next layer\n\n
 *
 * Used math functions:
 * * ailayer_leaky_relu.leaky_relu
 * * ailayer_leaky_relu.d_leaky_relu
 * * ailayer_leaky_relu.multiply
 *
 * @param *self Layer to calculate the backward path for.
 */
void ailayer_leaky_relu_backward(ailayer_t *self);

/** @brief Calculate the shape of the result tensor
 *
 * *Implementation of ailayer.calc_result_shape.*
 *
 * As the result tensor shape is shared with the result tensor shape of the previous layer (no change in shape is needed),
 * this function returns without doing anything.
 *
 * @param *self Layer to calculate the resulting shape for.
 */
void ailayer_leaky_relu_calc_result_shape(ailayer_t *self);

#ifdef AIDEBUG_PRINT_MODULE_SPECS
/** @brief Print the layer specification
 *
 * @param *self     The layer to print the specification for
 * @param *print    Pointer to the print function to use
 */
void ailayer_leaky_relu_print_specs(const ailayer_t *self, int (*print)(const char *format, ...));
#endif // AIDEBUG_PRINT_MODULE_SPECS

#endif // LEAKY_RELU_LAYER
