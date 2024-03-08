/*
 * 样式的标准模板
 *
 * The style's standard template.
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#ifndef __STYLE_TEMPLATE_H__
#define __STYLE_TEMPLATE_H__

using cus = const unsigned short;

static cus S_COLOR_RED  = 31;
static cus S_COLOR_BLUE = 36;

namespace style
{
    cus S_OPERATOR_COLOR = S_COLOR_BLUE;
    cus S_STRING_COLOR   = S_COLOR_BLUE;
    cus S_NUMBER_COLOR   = S_COLOR_BLUE;
    cus S_KEYWORD_COLOR  = S_COLOR_RED;
};

#endif