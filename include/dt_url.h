/*
 * =====================================================================================
 *
 *    Filename   :  dt_url.h
 *    Description:
 *    Version    :  1.0
 *    Created    :  2015��11��13�� 17ʱ59��43��
 *    Revision   :  none
 *    Compiler   :  gcc
 *    Author     :  peter-s (), peter_future@outlook.com
 *    Company    :  dt
 *
 * =====================================================================================
 */

#ifndef DT_URL_H
#define DT_URL_H

#include <string.h>

void dt_make_absolute_url(char *buf, int size, const char *base, const char *rel);

#endif
