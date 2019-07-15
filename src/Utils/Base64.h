#ifndef _BASE64_H_
#define _BASE64_H_

// ���  2011-01-12(Thu)
//��л��������¡�base64��������롷
//base64�ı���ԭ����ǽ�����������8bit�ַ�ת�����ĸ�6bit���ַ�.

// ���룬dest�Ŀռ��СΪ(src_len + 2) / 3 * 4 + 1��(+1�������һ���������Ŀռ�)
void	Base64Encode(char *dest, const unsigned char *src, int src_len);

// ���룬dest�Ŀռ��СΪsrc_len / 4 * 3��(src_len����4�ı���)
void	Base64Decode(unsigned char *dest, const char *src, int src_len);

#endif //_BASE64_H_