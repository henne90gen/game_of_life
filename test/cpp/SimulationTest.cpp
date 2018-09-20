#include "UnitTest++/UnitTest++.h"
#include "Simulation.h"
#include "Interface.h"
#include <utility>

double getTime() { return 0; }
double getRandom() { return 0; }

std::pair<Cell, std::map<std::string, Cell>> updateCell(Cell &cell, std::map<std::string, Cell> &map) {
    std::map<std::string, Cell> neighborMap;
    neighborMap["left"] = {species : cell.species, strength : cell.strength};
    neighborMap["right"] = {species : cell.species, strength : cell.strength};
    neighborMap["top"] = {species : cell.species, strength : cell.strength};
    neighborMap["bottom"] = {species : cell.species, strength : cell.strength};
    return std::pair<Cell, std::map<std::string, Cell>>(cell, neighborMap);
}

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

    TEST(SimpleNeighborPropagation) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[6] = {Species::blue, 100};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[6].species);
        CHECK_EQUAL(Species::blue, board.data[5].species);
        CHECK_EQUAL(Species::blue, board.data[7].species);
        CHECK_EQUAL(Species::blue, board.data[1].species);
        CHECK_EQUAL(Species::blue, board.data[11].species);
        CHECK_EQUAL(100, board.data[5].strength);
        CHECK_EQUAL(100, board.data[6].strength);
        CHECK_EQUAL(100, board.data[7].strength);
        CHECK_EQUAL(100, board.data[1].strength);
        CHECK_EQUAL(100, board.data[11].strength);
    }

    TEST(AdvancedNeighborPropagation) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[6] = {Species::blue, 100};
        board.data[7] = {Species::green, 50};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[6].species);
        CHECK_EQUAL(Species::blue, board.data[7].species);
        CHECK_EQUAL(50, board.data[6].strength);
        CHECK_EQUAL(50, board.data[7].strength);
    }

    TEST(ThreeWayNeighborPropagation) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[6] = {Species::blue, 100};
        board.data[7] = {Species::green, 50};
        board.data[8] = {Species::red, 75};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[6].species);
        CHECK_EQUAL(Species::red, board.data[7].species);
        CHECK_EQUAL(Species::red, board.data[8].species);
        CHECK_EQUAL(50, board.data[6].strength);
        CHECK_EQUAL(25, board.data[7].strength);
        CHECK_EQUAL(25, board.data[8].strength);
    }

    TEST(NeighborPropagationStopsAtBoardEdge) {
        Board board = {};
        board.width = 5;
        board.height = 5;
        board.data[4] = {Species::blue, 100};

        stepGame(&board);

        CHECK_EQUAL(Species::blue, board.data[4].species);
        CHECK_EQUAL(Species::none, board.data[5].species);
    }
}

int main(int argc, char *argv[]) { return UnitTest::RunAllTests(); }
