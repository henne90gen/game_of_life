#include "UnitTest++/UnitTest++.h"
#include "Simulation.h"
#include "Interface.h"

double getTime() { return 0; }
double getRandom() { return 0; }

SUITE(GameOfLife) {
    TEST(EmptyCellDoesNothing) {
        Board board = {};
        board.width = 5;
        board.height = 5;

        stepGame(&board);

        for (int i = 0; i < board.height * board.width; i++) {
            CHECK_EQUAL(Species::none, board.data[i].species);
            CHECK_EQUAL(0, board.data[i].strength);
        }
    }

    TEST(LiveCellSplitsItself) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = {Species::blue, 100};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[0].species);
        CHECK_EQUAL(Species::blue, board.data[1].species);
        CHECK_EQUAL(75, board.data[0].strength);
        CHECK_EQUAL(25, board.data[1].strength);
    }

    TEST(LiveCellSpillsOverToCellOfOtherSpecies) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[0] = {Species::blue, 100};
        board.data[1] = {Species::green, 100};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[0].species);
        CHECK_EQUAL(Species::green, board.data[1].species);
        CHECK_EQUAL(75, board.data[0].strength);
        CHECK_EQUAL(75, board.data[1].strength);
    }
}

int main(int, const char *[]) { return UnitTest::RunAllTests(); }
