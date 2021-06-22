/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class dv_DoubleVector */

#ifndef _Included_dv_DoubleVector
#define _Included_dv_DoubleVector
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     dv_DoubleVector
 * Method:    DV_SetCUDDManager
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1SetCUDDManager
  (JNIEnv *, jclass, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_CreateZeroVector
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1CreateZeroVector
  (JNIEnv *, jobject, jint);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_ConvertMTBDD
 * Signature: (JJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1ConvertMTBDD
  (JNIEnv *, jobject, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_GetElement
 * Signature: (JII)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1GetElement
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SetElement
 * Signature: (JIID)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1SetElement
  (JNIEnv *, jobject, jlong, jint, jint, jdouble);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SetAllElements
 * Signature: (JID)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1SetAllElements
  (JNIEnv *, jobject, jlong, jint, jdouble);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_RoundOff
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1RoundOff
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SubtractFromOne
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1SubtractFromOne
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_Add
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1Add
  (JNIEnv *, jobject, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_TimesConstant
 * Signature: (JID)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1TimesConstant
  (JNIEnv *, jobject, jlong, jint, jdouble);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_DotProduct
 * Signature: (JIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1DotProduct
  (JNIEnv *, jobject, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_Filter
 * Signature: (JJDJIJ)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1Filter
  (JNIEnv *, jobject, jlong, jlong, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_MaxMTBDD
 * Signature: (JJJIJ)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1MaxMTBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_Clear
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1Clear
  (JNIEnv *, jobject, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_GetNNZ
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_dv_DoubleVector_DV_1GetNNZ
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_FirstFromBDD
 * Signature: (JJJIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1FirstFromBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_MinOverBDD
 * Signature: (JJJIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1MinOverBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_MaxOverBDD
 * Signature: (JJJIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1MaxOverBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SumOverBDD
 * Signature: (JJJIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1SumOverBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SumOverMTBDD
 * Signature: (JJJIJ)D
 */
JNIEXPORT jdouble JNICALL Java_dv_DoubleVector_DV_1SumOverMTBDD
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_SumOverDDVars
 * Signature: (JJJIIIJJ)V
 */
JNIEXPORT void JNICALL Java_dv_DoubleVector_DV_1SumOverDDVars
  (JNIEnv *, jobject, jlong, jlong, jlong, jint, jint, jint, jlong, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDGreaterThanEquals
 * Signature: (JDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDGreaterThanEquals
  (JNIEnv *, jobject, jlong, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDGreaterThan
 * Signature: (JDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDGreaterThan
  (JNIEnv *, jobject, jlong, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDLessThanEquals
 * Signature: (JDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDLessThanEquals
  (JNIEnv *, jobject, jlong, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDLessThan
 * Signature: (JDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDLessThan
  (JNIEnv *, jobject, jlong, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDInterval
 * Signature: (JDDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDInterval
  (JNIEnv *, jobject, jlong, jdouble, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDCloseValueAbs
 * Signature: (JDDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDCloseValueAbs
  (JNIEnv *, jobject, jlong, jdouble, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_BDDCloseValueRel
 * Signature: (JDDJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1BDDCloseValueRel
  (JNIEnv *, jobject, jlong, jdouble, jdouble, jlong, jint, jlong);

/*
 * Class:     dv_DoubleVector
 * Method:    DV_ConvertToMTBDD
 * Signature: (JJIJ)J
 */
JNIEXPORT jlong JNICALL Java_dv_DoubleVector_DV_1ConvertToMTBDD
  (JNIEnv *, jobject, jlong, jlong, jint, jlong);

#ifdef __cplusplus
}
#endif
#endif
