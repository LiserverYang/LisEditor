/*
 * 样式的标准模板
 *
 * The standard template of style.
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#pragma once

#ifndef __STYLE_TEMPLATE_H__
#define __STYLE_TEMPLATE_H__

using cus = const unsigned short;

static cus S_COLOR_RED  = 31;
static cus S_COLOR_BLUE = 36;

namespace style
{
    /*
     * The custom style here.
     */
    cus S_OPERATOR_COLOR = S_COLOR_BLUE;
    cus S_STRING_COLOR   = S_COLOR_BLUE;
    cus S_NUMBER_COLOR   = S_COLOR_BLUE;
    cus S_KEYWORD_COLOR  = S_COLOR_RED;
};

#endif // __STYLE_TEMPLATE_H__