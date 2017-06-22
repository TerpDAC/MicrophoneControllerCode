/**
 * @file   MicSenseComm.h
 * @brief  MicSense data uploading functions (header)
 *
 * Provides functions related to uploading the MicSense data, as well
 * as fetching calibration from the MicSense server. It also includes
 * private helper functions for (somewhat) reliable HTTP handling.
 * 
 * This is the header file for these functions. See source for actual
 * documentation.
 * 
 * @author MicSense Team / Terrapin Development and Consulting
 * @date   May 2017
 */
/*====================================================================*/
#ifndef _MICSENSE_COMM_H
#define _MICSENSE_COMM_H
void submitSum(uint32_t curTime, long totalSampleCount, long highTotalCount, long midTotalCount);
void getCalibration();
#endif
