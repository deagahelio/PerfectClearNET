#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/moves.hpp"

namespace core {
    using namespace std::literals::string_literals;

    class MovesTest : public ::testing::Test {
    };

    bool assertMove(std::vector<Move> &moves, const Move &move) {
        auto result = std::find(moves.begin(), moves.end(), move);
        return result != moves.end();
    }

    TEST_F(MovesTest, case1) {
        auto field = createField(
                "XXXX____XX"s +
                "XXXX___XXX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::T, 4);

            EXPECT_EQ(moves.size(), 7);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 0}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 4, 1}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Right, 5, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 5, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Reverse, 6, 3}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1}));
        }

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 4);

            EXPECT_EQ(moves.size(), 3);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 5, 1}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 2}));
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Left, 5, 1}));
        }
    }

    TEST_F(MovesTest, case2) {
        auto field = createField(
                "XXXXXX__XX"s +
                "XXXXX__XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 2);

            EXPECT_EQ(moves.size(), 1);
            EXPECT_TRUE(assertMove(moves, Move{RotateType::Spawn, 6, 0}));
        }
    }

    TEST_F(MovesTest, case3) {
        auto field = createField(
                "XXXX_X__XX"s +
                "XXXX__XXXX"s +
                "XXXX___XXX"s +
                ""
        );

        auto factory = Factory::create();
        auto generator = srs::MoveGenerator(factory);

        {
            auto moves = std::vector<Move>();
            generator.search(moves, field, PieceType::S, 3);

            EXPECT_EQ(moves.size(), 0);
        }
    }

    // x列より右の列を選択するマスクを作成（x列を含む）
    uint64_t right(int minX) {
        assert(0 <= minX && minX <= 10);
        switch (minX) {
            case 0:
                return 0xfffffffffffffffL;
            case 1:
                return 0xffbfeffbfeffbfeL;
            case 2:
                return 0xff3fcff3fcff3fcL;
            case 3:
                return 0xfe3f8fe3f8fe3f8L;
            case 4:
                return 0xfc3f0fc3f0fc3f0L;
            case 5:
                return 0xf83e0f83e0f83e0L;
            case 6:
                return 0xf03c0f03c0f03c0L;
            case 7:
                return 0xe0380e0380e0380L;
            case 8:
                return 0xc0300c0300c0300L;
            case 9:
                return 0x802008020080200L;
            case 10:
                return 0L;
            default:
                assert(false);
                return 0L;
        }
    }

    // x列より左の列を選択するマスクを作成（x列を含まない）
    uint64_t left(int minX) {
        assert(0 <= minX && minX <= 10);
        switch (minX) {
            case 0:
                return 0L;
            case 1:
                return 0x004010040100401ULL;
            case 2:
                return 0x00c0300c0300c03ULL;
            case 3:
                return 0x01c0701c0701c07ULL;
            case 4:
                return 0x03c0f03c0f03c0fULL;
            case 5:
                return 0x07c1f07c1f07c1fULL;
            case 6:
                return 0x0fc3f0fc3f0fc3fULL;
            case 7:
                return 0x1fc7f1fc7f1fc7fULL;
            case 8:
                return 0x3fcff3fcff3fcffULL;
            case 9:
                return 0x7fdff7fdff7fdffULL;
            case 10:
                return 0xfffffffffffffffULL;
            default:
                assert(false);
                return 0L;
        }
    }

    void g(int x, int y, const Bitboard original[], Bitboard boards[], Bitboard down[]) {
        const auto y0Block = 0x3ffULL;
        const auto y01Block = 0xfffffULL;
        const auto y012Block = 0x3fffffffULL;

        switch (x + 4 * y) {
            // y=2
            case 8: {
                assert(x == 0 && y == 2);

                const auto slide = 20;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
                const auto block = left(mod);  // _____ ____
                const auto empty2 = ~block & y0Block;    // XXXXX XXXXX
                const auto fw = FIELD_WIDTH;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) >> slide) | ((original[1] & y01Block) << aslide);
                boards[1] |= ((original[1]) >> slide) | ((original[2] & y01Block) << aslide);
                boards[2] |= ((original[2]) >> slide) | ((original[3] & y01Block) << aslide);
                boards[3] |= ((original[3]) >> slide);

                boards[0] |= ((original[0]) >> fw) | ((original[1] & y0Block) << afw);
                boards[1] |= ((original[1]) >> fw) | ((original[2] & y0Block) << afw);
                boards[2] |= ((original[2]) >> fw) | ((original[3] & y0Block) << afw);
                boards[3] |= ((original[3]) >> fw);

                break;
            }
                // y=1
            case 5: {
                assert(x == 1 && y == 1);

                const auto slide = 11;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
                const auto empty = right(mod);  // _XXXX XXXXX
                const auto block = right(10 - mod);    // _____ ____X
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0] & empty) >> slide) | ((original[1] & empty) << aslide);
                boards[1] |= ((original[1] & empty) >> slide) | ((original[2] & empty) << aslide);
                boards[2] |= ((original[2] & empty) >> slide) | ((original[3] & empty) << aslide);
                boards[3] |= ((original[3] & empty) >> slide);

                down[0] |= ((original[0] & empty) >> mod);
                down[1] |= ((original[1] & empty) >> mod);
                down[2] |= ((original[2] & empty) >> mod);
                down[3] |= ((original[3] & empty) >> mod);

                break;
            }
            case 4: {
                assert(x == 0 && y == 1);

                const auto slide = 10;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
                const auto block = left(mod);  // _____ ____
                const auto empty2 = ~block & y0Block;    // XXXXX XXXXX
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) >> slide) | ((original[1] & y0Block) << aslide);
                boards[1] |= ((original[1]) >> slide) | ((original[2] & y0Block) << aslide);
                boards[2] |= ((original[2]) >> slide) | ((original[3] & y0Block) << aslide);
                boards[3] |= ((original[3]) >> slide);

                down[0] |= original[0];
                down[1] |= original[1];
                down[2] |= original[2];
                down[3] |= original[3];

                break;
            }
            case 3: {
                assert(x == -1 && y == 1);

                const auto slide = 9;
                const auto aslide = 60 - slide;
                const auto mod = 1;
                const auto empty = left(10 - mod);  // XXXXX XXXX_
                const auto block = left(mod);  // X____ ____
                const auto empty2 = right(1);    // _XXXX XXXXX
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) >> slide) | ((original[1] & y0Block) << aslide);
                boards[1] |= ((original[1]) >> slide) | ((original[2] & y0Block) << aslide);
                boards[2] |= ((original[2]) >> slide) | ((original[3] & y0Block) << aslide);
                boards[3] |= ((original[3]) >> slide);

                down[0] |= ((original[0] & empty2) << 1);
                down[1] |= ((original[1] & empty2) << 1);
                down[2] |= ((original[2] & empty2) << 1);
                down[3] |= ((original[3] & empty2) << 1);

                break;
            }
                // y=0
            case 2: {
                assert(x == 2 && y == 0);
                const auto slide = 2;
                const auto mod = slide % 10;
                const auto empty = right(mod);  // __XXX XXXXX
                const auto block = right(10 - mod);    // _____ ___XX
                const auto empty2 = ~block & y0Block;    // XXXXX XXX__
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0] & empty) >> slide) | block;
                boards[1] |= ((original[1] & empty) >> slide) | block;
                boards[2] |= ((original[2] & empty) >> slide) | block;
                boards[3] |= ((original[3] & empty) >> slide) | block;

                down[0] |= ((original[0]) << fw) | y01Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y0Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y0Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y0Block);

                break;
            }
            case 1: {
                assert(x == 1 && y == 0);

                const auto slide = 1;
                const auto mod = slide % 10;
                const auto empty = right(mod);
                const auto block = right(10 - mod);
                const auto empty2 = ~block & y0Block;    // XXXXX XXX__
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0] & empty) >> slide) | block;
                boards[1] |= ((original[1] & empty) >> slide) | block;
                boards[2] |= ((original[2] & empty) >> slide) | block;
                boards[3] |= ((original[3] & empty) >> slide) | block;

                down[0] |= ((original[0] & empty) << fw) | empty2;
                down[1] |= ((original[1] & empty) << fw) | ((original[0] >> afw) & y0Block);
                down[2] |= ((original[2] & empty) << fw) | ((original[1] >> afw) & y0Block);
                down[3] |= ((original[3] & empty) << fw) | ((original[2] >> afw) & y0Block);

                break;
            }
            case 0: {
                assert(x == 0 && y == 0);

                const auto slide = 0;
                const auto fw = FIELD_WIDTH - slide;
                const auto afw = 60 - fw;

                // no op for boards

                down[0] |= ((original[0]) << fw) | y0Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y0Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y0Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y0Block);

                break;
            }
            case -1: {
                assert(x == -1 && y == 0);

                const auto slide = 1;
                const auto mod = slide % 10;
                const auto empty = left(10 - mod);  // XXXXX XXXX_
                const auto block = left(mod);  // X____ ____
                const auto empty2 = ~block & y0Block;    // _XXXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0] & empty) << slide) | block;
                boards[1] |= ((original[1] & empty) << slide) | block;
                boards[2] |= ((original[2] & empty) << slide) | block;
                boards[3] |= ((original[3] & empty) << slide) | block;

                down[0] |= ((original[0] & empty) << fw) | empty2;
                down[1] |= ((original[1] & empty) << fw) | ((original[0] >> afw) & y0Block);
                down[2] |= ((original[2] & empty) << fw) | ((original[1] >> afw) & y0Block);
                down[3] |= ((original[3] & empty) << fw) | ((original[2] >> afw) & y0Block);

                break;
            }
            case -2: {
                assert(x == -2 && y == 0);

                const auto slide = 2;
                const auto mod = slide % 10;
                const auto empty = left(10 - mod);  // XXXXX XXX__
                const auto block = left(mod);  // XX___ ____
                const auto empty2 = ~block & y0Block;    // __XXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0] & empty) << slide) | block;
                boards[1] |= ((original[1] & empty) << slide) | block;
                boards[2] |= ((original[2] & empty) << slide) | block;
                boards[3] |= ((original[3] & empty) << slide) | block;

                down[0] |= ((original[0] & empty) << fw) | empty2;
                down[1] |= ((original[1] & empty) << fw) | ((original[0] >> afw) & y0Block);
                down[2] |= ((original[2] & empty) << fw) | ((original[1] >> afw) & y0Block);
                down[3] |= ((original[3] & empty) << fw) | ((original[2] >> afw) & y0Block);

                break;
            }
                // y=-1
            case -3: {
                assert(x == 1 && y == -1);

                const auto slide = 11;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
//                const auto block = left(mod);  // XX___ ____
//                const auto empty2 = ~block & y0Block;    // __XXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) << slide) | y0Block;
                boards[1] |= ((original[1]) << slide) | ((original[0] >> aslide) & y0Block);
                boards[2] |= ((original[2]) << slide) | ((original[1] >> aslide) & y0Block);
                boards[3] |= ((original[3]) << slide) | ((original[2] >> aslide) & y0Block);

                down[0] |= ((original[0]) << fw) | y01Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y01Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y01Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y01Block);

                break;
            }
            case -4: {
                assert(x == 0 && y == -1);

                const auto slide = 10;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
//                const auto block = left(mod);  // XX___ ____
//                const auto empty2 = ~block & y0Block;    // __XXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) << slide) | y0Block;
                boards[1] |= ((original[1]) << slide) | ((original[0] >> aslide) & y0Block);
                boards[2] |= ((original[2]) << slide) | ((original[1] >> aslide) & y0Block);
                boards[3] |= ((original[3]) << slide) | ((original[2] >> aslide) & y0Block);

                down[0] |= ((original[0]) << fw) | y01Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y01Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y01Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y01Block);

                break;
            }
            case -5: {
                assert(x == -1 && y == -1);

                const auto slide = 9;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
//                const auto block = left(mod);  // XX___ ____
//                const auto empty2 = ~block & y0Block;    // __XXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) << slide) | y0Block;
                boards[1] |= ((original[1]) << slide) | ((original[0] >> aslide) & y0Block);
                boards[2] |= ((original[2]) << slide) | ((original[1] >> aslide) & y0Block);
                boards[3] |= ((original[3]) << slide) | ((original[2] >> aslide) & y0Block);

                down[0] |= ((original[0]) << fw) | y01Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y01Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y01Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y01Block);

                break;
            }
                // y=-2
            case -8: {
                assert(x == 0 && y == -2);

                const auto slide = 20;
                const auto aslide = 60 - slide;
                const auto mod = slide % 10;
//                const auto block = left(mod);  // XX___ ____
//                const auto empty2 = ~block & y0Block;    // __XXX XXXXX
                const auto fw = FIELD_WIDTH + slide;
                const auto afw = 60 - fw;

                boards[0] |= ((original[0]) << slide) | y0Block;
                boards[1] |= ((original[1]) << slide) | ((original[0] >> aslide) & y01Block);
                boards[2] |= ((original[2]) << slide) | ((original[1] >> aslide) & y01Block);
                boards[3] |= ((original[3]) << slide) | ((original[2] >> aslide) & y01Block);

                down[0] |= ((original[0]) << fw) | y01Block;
                down[1] |= ((original[1]) << fw) | ((original[0] >> afw) & y012Block);
                down[2] |= ((original[2]) << fw) | ((original[1] >> afw) & y012Block);
                down[3] |= ((original[3]) << fw) | ((original[2] >> afw) & y012Block);

                break;
            }
            default: {
                assert(false);
                break;
            }
        }
    }

    TEST_F(MovesTest, case4) {
//        auto field = createField(
//                "XX________"s +
//                "XXX_______"s +
//                "XXXX______"s +
//                "XXXXX_____"s +
//                "XXXXXX____"s +
//                "XXXXXXX___"s +
//                "XXXXXXXX__"s +
//                "XXXXXXXXX_"s +
//                ""
//        );

        auto field = createField(
                "____X_____"s +
                "__________"s +
                ""
        );

        Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
        Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
        Bitboard down[4] = {0, 0, 0, 0};

        g(-1, 0, original, boards, down);
        g(0, 0, original, boards, down);
        g(1, 0, original, boards, down);
        g(2, 0, original, boards, down);

        std::cout << std::endl;

        for (int i = 0; i < 4; ++i) {
            field.boards[i] = ~boards[i];
        }
        std::cout << field.toString(24) << std::endl;

        std::cout << "=========" << std::endl;

        for (int i = 0; i < 4; ++i) {
            field.boards[i] = down[i];
        }
        std::cout << field.toString(24) << std::endl;

        std::cout << "=========" << std::endl;

        for (int i = 0; i < 4; ++i) {
            field.boards[i] = ~boards[i] & down[i];
        }
        std::cout << field.toString(24) << std::endl;
    }

    TEST_F(MovesTest, caseI) {
        auto field = createField(
                "____X_____"s +
                "__________"s +
                ""
        );

//        {
//            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard down[4] = {0, 0, 0, 0};
//
//            g(-1, 0, original, boards, down);
//            g(0, 0, original, boards, down);
//            g(1, 0, original, boards, down);
//            g(2, 0, original, boards, down);
//
//            for (int i = 0; i < 4; ++i) {
//                field.boards[i] = ~boards[i] & down[i];
//            }
//            std::cout << field.toString(24) << std::endl;
//        }

        {
            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard down[4] = {0, 0, 0, 0};

            g(0, -1, original, boards, down);
            g(0, 0, original, boards, down);
            g(0, 1, original, boards, down);
            g(0, 2, original, boards, down);

            for (int i = 0; i < 4; ++i) {
                field.boards[i] = ~boards[i] & down[i];
            }
            std::cout << field.toString(24) << std::endl;
        }
    }

    TEST_F(MovesTest, caseT) {
        auto field = createField(
                "____X_____"s +
                "__________"s +
                ""
        );

//        {
//            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard down[4] = {0, 0, 0, 0};
//
//            g(0, -1, original, boards, down);
//            g(0, 0, original, boards, down);
//            g(1, 0, original, boards, down);
//            g(-1, 0, original, boards, down);
//
//            for (int i = 0; i < 4; ++i) {
//                field.boards[i] = ~boards[i] & down[i];
//            }
//            std::cout << field.toString(24) << std::endl;
//        }

        {
            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard down[4] = {0, 0, 0, 0};

            g(0, 1, original, boards, down);
            g(0, 0, original, boards, down);
            g(1, 0, original, boards, down);
            g(-1, 0, original, boards, down);

            for (int i = 0; i < 4; ++i) {
                field.boards[i] = ~boards[i] & down[i];
            }
            std::cout << field.toString(24) << std::endl;
        }
    }

    TEST_F(MovesTest, caseS) {
        auto field = createField(
                "____X_____"s +
                "__________"s +
                ""
        );

//        {
//            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard down[4] = {0, 0, 0, 0};
//
//            g(-1, 0, original, boards, down);
//            g(0, 0, original, boards, down);
//            g(0, 1, original, boards, down);
//            g(1, 1, original, boards, down);
//
//            for (int i = 0; i < 4; ++i) {
//                field.boards[i] = ~boards[i] & down[i];
//            }
//            std::cout << field.toString(24) << std::endl;
//        }

//        {
//            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard down[4] = {0, 0, 0, 0};
//
//            g(-1, 1, original, boards, down);
//            g(0, 0, original, boards, down);
//            g(1, 0, original, boards, down);
//            g(0, 1, original, boards, down);
//
//            for (int i = 0; i < 4; ++i) {
//                field.boards[i] = ~boards[i] & down[i];
//            }
//            std::cout << field.toString(24) << std::endl;
//        }

//        {
//            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
//            Bitboard down[4] = {0, 0, 0, 0};
//
//            g(0, 0, original, boards, down);
//            g(1, 0, original, boards, down);
//            g(0, -1, original, boards, down);
//            g(-1, -1, original, boards, down);
//
//            for (int i = 0; i < 4; ++i) {
//                field.boards[i] = ~boards[i] & down[i];
//            }
//            std::cout << field.toString(24) << std::endl;
//        }

        {
            Bitboard original[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard boards[4] = {field.boards[0], field.boards[1], field.boards[2], field.boards[3]};
            Bitboard down[4] = {0, 0, 0, 0};

            g(0, 0, original, boards, down);
            g(-1, 0, original, boards, down);
            g(0, -1, original, boards, down);
            g(1, -1, original, boards, down);

            for (int i = 0; i < 4; ++i) {
                field.boards[i] = ~boards[i] & down[i];
            }
            std::cout << field.toString(24) << std::endl;
        }
    }
}
