#pragma once

namespace kmpFind {
	unsigned char HexToI(char *hex);
	int Find(const unsigned char *text, int text_len, const char *patn);
}
