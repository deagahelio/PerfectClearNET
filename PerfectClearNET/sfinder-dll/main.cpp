#include "main.hpp"

#pragma unmanaged

core::PieceType charToPiece(char x) {
	switch (x) {
		case 'S':
			return core::PieceType::S;

		case 'Z':
			return core::PieceType::Z;

		case 'J':
			return core::PieceType::J;

		case 'L':
			return core::PieceType::L;

		case 'T':
			return core::PieceType::T;

		case 'O':
			return core::PieceType::O;

		case 'I':
			return core::PieceType::I;
	}
}

DLL void action(const char* _field, const char* _queue, const char* _hold, char* _str, int _len) {
	auto field = core::createField(_field);

	auto pieces = std::vector<core::PieceType>();

	bool holdEmpty = _hold[0] == 'E';

	if (!holdEmpty)
		pieces.push_back(charToPiece(_hold[0]));
	
	for (const char* p = &_queue[0]; *p != '\0'; ++p)
		pieces.push_back(charToPiece(*p));

	auto result = pcfinder.run(field, pieces, pieces.size(), 6, holdEmpty);

	std::stringstream out;

	if (!result.empty())
		for (const auto &item : result)
			out << item.pieceType << ","
				<< item.rotateType << ","
				<< item.x << ","
				<< item.y << "|";

	else out << "-1";

	std::string a = out.str();
	std::copy(a.c_str(), a.c_str() + a.length() + 1, _str);
}

BOOL WINAPI DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
	return TRUE;
}