#include "UnitTest++/UnitTest++.h"
#include "Simulation.h"
#include "Interface.h"

double getTime() { return 0; }

SUITE(GameOfLife) {
    TEST(LiveCellWithLessThanTwoLiveNeighbors) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[0], false);

        board.data[0] = true;
        board.data[1] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[0], false);
        CHECK_EQUAL(board.data[1], false);
    }

    TEST(LiveCellWithTwoLiveNeighbors) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = true;
        board.data[1] = true;
        board.data[2] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[0], false);
        CHECK_EQUAL(board.data[1], true);
        CHECK_EQUAL(board.data[2], false);
    }

    TEST(LiveCellWithThreeLiveNeighbors) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = true;
        board.data[1] = true;
        board.data[2] = true;
        board.data[6] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[0], true);
        CHECK_EQUAL(board.data[1], true);
        CHECK_EQUAL(board.data[2], true);
        CHECK_EQUAL(board.data[6], true);
    }

    TEST(LiveCellWithMoreThanThreeLiveNeighbors) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = true;
        board.data[1] = true;
        board.data[2] = true;
        board.data[5] = true;
        board.data[6] = true;
        board.data[7] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[0], true);
        CHECK_EQUAL(board.data[1], false);
        CHECK_EQUAL(board.data[2], true);
        CHECK_EQUAL(board.data[5], true);
        CHECK_EQUAL(board.data[6], false);
        CHECK_EQUAL(board.data[7], true);
    }

    TEST(DeadCellWithThreeLiveNeighbors) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = true;
        board.data[1] = true;
        board.data[5] = true;

        stepGameOfLife(&board);

        CHECK_EQUAL(board.data[6], true);
    }
}

int main(int, const char *[]) { return UnitTest::RunAllTests(); }
