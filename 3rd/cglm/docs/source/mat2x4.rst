.. default-domain:: C

mat2x4
======

Header: cglm/mat2x4.h

Table of contents (click to go):
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Macros:

1. GLM_MAT2X4_ZERO_INIT
#. GLM_MAT2X4_ZERO

Functions:

1. :c:func:`glm_mat2x4_copy`
#. :c:func:`glm_mat2x4_zero`
#. :c:func:`glm_mat2x4_make`
#. :c:func:`glm_mat2x4_mul`
#. :c:func:`glm_mat2x4_mulv`
#. :c:func:`glm_mat2x4_transpose`
#. :c:func:`glm_mat2x4_scale`

Functions documentation
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: void glm_mat2x4_copy(mat2x4 mat, mat2x4 dest)

    copy mat2x4 to another one (dest).

    Parameters:
      | *[in]*  **mat**   source
      | *[out]* **dest**  destination

.. c:function:: void glm_mat2x4_zero(mat2x4 mat)

    make given matrix zero

    Parameters:
      | *[in,out]* **mat**  matrix

.. c:function:: void glm_mat2x4_make(const float * __restrict src, mat2x4 dest)

    Create mat2x4 matrix from pointer

    .. note:: **@src** must contain at least 8 elements.

    Parameters:
      | *[in]*  **src**  pointer to an array of floats
      | *[out]* **dest** destination matrix2x4

.. c:function:: void glm_mat2x4_mul(mat2x4 m1, mat4x2 m2, mat2 dest)

    multiply m1 and m2 to dest

    .. code-block:: c

       glm_mat2x4_mul(mat2x4, mat4x2, mat2);

    Parameters:
      | *[in]*  **m1**    left matrix (mat2x4)
      | *[in]*  **m2**    right matrix (mat4x2)
      | *[out]* **dest**  destination matrix (mat2)

.. c:function:: void glm_mat2x4_mulv(mat2x4 m, vec4 v, vec2 dest)

    multiply mat2x4 with vec4 (column vector) and store in dest vector

    Parameters:
      | *[in]*  **m**     mat2x4 (left)
      | *[in]*  **v**     vec4 (right, column vector)
      | *[out]* **dest**  destination (result, column vector)

.. c:function:: void glm_mat2x4_transpose(mat2x4 m, mat4x2 dest)

    transpose matrix and store in dest

    Parameters:
      | *[in]*  **m**     matrix
      | *[out]* **dest**  destination

.. c:function:: void  glm_mat2x4_scale(mat2x4 m, float s)

    multiply matrix with scalar

    Parameters:
      | *[in, out]* **m** matrix
      | *[in]*      **s** scalar
