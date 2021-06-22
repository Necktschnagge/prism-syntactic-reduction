/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class sparse_NDSparseMatrix */

#ifndef _Included_sparse_NDSparseMatrix
#define _Included_sparse_NDSparseMatrix
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     sparse_NDSparseMatrix
 * Method:    PS_NDGetActionIndex
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_sparse_NDSparseMatrix_PS_1NDGetActionIndex
  (JNIEnv *, jclass, jlong, jint, jint);

/*
 * Class:     sparse_NDSparseMatrix
 * Method:    PS_BuildNDSparseMatrix
 * Signature: (JJJIJIJI)J
 */
JNIEXPORT jlong JNICALL Java_sparse_NDSparseMatrix_PS_1BuildNDSparseMatrix
  (JNIEnv *, jclass, jlong, jlong, jlong, jint, jlong, jint, jlong, jint);

/*
 * Class:     sparse_NDSparseMatrix
 * Method:    PS_BuildSubNDSparseMatrix
 * Signature: (JJJIJIJIJ)J
 */
JNIEXPORT jlong JNICALL Java_sparse_NDSparseMatrix_PS_1BuildSubNDSparseMatrix
  (JNIEnv *, jclass, jlong, jlong, jlong, jint, jlong, jint, jlong, jint, jlong);

/*
 * Class:     sparse_NDSparseMatrix
 * Method:    PS_AddActionsToNDSparseMatrix
 * Signature: (JJJJIJIJIJ)V
 */
JNIEXPORT void JNICALL Java_sparse_NDSparseMatrix_PS_1AddActionsToNDSparseMatrix
  (JNIEnv *, jclass, jlong, jlong, jlong, jlong, jint, jlong, jint, jlong, jint, jlong);

/*
 * Class:     sparse_NDSparseMatrix
 * Method:    PS_DeleteNDSparseMatrix
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_sparse_NDSparseMatrix_PS_1DeleteNDSparseMatrix
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
