/***************************************************************************************
 *
 * This program solves the 2D puzzle "IQ Fit".
 * Copyright (C) 2016  Dominik Vilsmeier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************************/

#include <stdio.h>
#include <stdlib.h>

struct Piece {
	/**
	 * By default (unrotated state) the piece's larger extent (3 or 4) points in y-direction.
	 * Parts of the piece are denoted by 1s, gaps (in the enclosing rectangle) by 0s.
	 */
	short A[4][2];  // version A;
	short B[4][2];  // version B;
	short (*version)[4][2];  // current version (either A or B);
	short y_range;  // the actual length of the piece in y-direction;
	short x_range_A[4];  // number of non-zero elements in the top row for each rotation; allows for excluding rotations;
	short x_range_B[4];  // number of non-zero elements in the top row for each rotation;
	short* x_range;  // current x_range (either x_range_A or x_range_B);
	int used;  // indicates whether the piece was already used and in which configuration (0: unused, <version>*1000 + <rotation>*100 + <y>*10 + <x>: oterhwise (where <version> is 1 for A and 2 for B));
	short skip;  // indicates whether the piece should be skipped for the current row;
};

void create_pieces(struct Piece* pieces);
void iter_rows(short board[][5], struct Piece* pieces, short const which_row, FILE* fp_constellations);
int place_piece_on_board(short board[][5], struct Piece* piece, short const which_row, short const x0, short const rotation);
void remove_piece_from_board(short board[][5], struct Piece* piece, short const which_row, short const x0, short const rotation);

int main (int argc, char** argv) {

	short board[10][5];  // primary index: y-direction (10), secondary: x-direction (5);
						 // occupied sites are denoted with 1s, free sites with 0s;
	for(short i=0; i<10; ++i) {
		for(short j=0; j<5; ++j) {
			board[i][j] = 0;
		}
	}

	// struct Piece pieces[10];
	struct Piece* pieces = calloc(10, sizeof *pieces);

	create_pieces(pieces);

	FILE* fp_constellations = fopen("constellations.txt", "w");  // will contain all possible solutions;

	iter_rows(board, pieces, 0, fp_constellations);

	fclose(fp_constellations);

	return 0;
}


/**
 * Writes a valid combination to the file.
 * 
 * @param fp -- pointer to the output file.
 * @param pieces -- pointer to the array of all pieces.
 *
 */
void write_combination_to_file(FILE* fp, struct Piece* pieces) {
	fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", pieces[0].used, pieces[1].used, pieces[2].used, pieces[3].used, pieces[4].used, pieces[5].used, pieces[6].used, pieces[7].used, pieces[8].used, pieces[9].used);
}


/**
 * Iterates over the rows of the board, trying to complete one by one.
 *
 * @param board -- pointer to the two-dimensional array representing the board.
 * @param pieces -- pointer to the array of all pieces.
 * @param which_row -- indicates which row the algorithm is currently working on.
 * @param fp_constellations -- pointer to the output file.
 *
 */
void iter_rows(short board[][5], struct Piece* pieces, short const which_row, FILE* fp_constellations) {

	short nopen=5;  // number of free sites in the current row;
	for(short j=0; j<5; ++j) {
		nopen -= board[which_row][j];
	}

	if(nopen == 0) {  // row is already complete;
		if(which_row == 9) {  // is the last row;
			for(short i=0; i<10; ++i) {
				if(pieces[i].used == 0) pieces[i].skip = 1;  // set each unused piece on skip because skipped pieces are set on unskip after a row was completed in order to make them available for the next row. However there is no additional row after the last one and in case of solutions including only 9 pieces having unused, unskipped pieces can result in an infinite loop (as they will be used in place of the piece that completed the board in subsequent steps which makes it possible to use the completing piece again). Therefore skip them in case the algorithm found a solution.
			}
			write_combination_to_file(fp_constellations, pieces);
		} else {  // is not the last row;
			iter_rows(board, pieces, which_row+1, fp_constellations);  // move on to the next row;
		}
		return;
	}

	short nused=0;  // how many successive pieces (without gap, from the beginning) have already been used or are currently on skip. At the same time this indicates the position of the piece that will be used this step withing the array of pieces.
	struct Piece* piece = pieces;
	while((piece->used > 0 || piece->skip > 0) && nused<10) {
		piece += 1;
		nused += 1;
	}
	if(nused == 10) return;  // All pieces are either used or on skip for the current row -> no solution could be found.

	short x_max;  // x-value of the 'rightmost' site the piece can be placed on in order not to exceed the boundaries of the board;
	int success;  // indicates whether the current piece was placed successfully on the board;

	for(short rotation=0; rotation<4; ++rotation) {  // loop over rotations; rotation are to be regarded clockwise;

		if(rotation%2 == 0) {  // 0 degrees or 180 degrees;
			if(which_row + piece->y_range > 10) continue;  // piece would exceed y-range of the board;
			x_max = 3;  // x-extent of piece is 2;
		} else {  // 90 degrees or 270 degrees;
			if(which_row + 2 > 10) continue;  // piece would exceed y-range of the board;
			x_max = 5-piece->y_range;  // original y-range becomes x-range;
		}

		piece->x_range = piece->x_range_A;  // use version A for x_range;
		if(piece->x_range[rotation] <= nopen) {  // check if there are enough free sites in the current row;

			piece->version = &(piece->A);  // use version A;
			for(short x=0; x<=x_max; ++x) {  // x denotes the x-position of the left upper corner of the enclosing rectangle within which the piece will be placed;

				success = place_piece_on_board(board, piece, which_row, x, rotation);
				if(success == 0) {  // no overlap of pieces;

					piece->used = 1000 + 100*rotation + 10*which_row + x;  // mark piece as used (see declaration of Piece for encoding);
					nopen -= piece->x_range[rotation];  // adjust number of free sites;
					if(nopen == 0) {  // current row is complete;

						for(short i=0; i<10; ++i) {
							pieces[i].skip = 0;  // reset skip of pieces in order to make them available for the next row;
						}
						iter_rows(board, pieces, which_row+1, fp_constellations);  // move on to the next row;
					} else {  // row is not complete;

						iter_rows(board, pieces, which_row, fp_constellations);  // stay within the current row;
					}
					remove_piece_from_board(board, piece, which_row, x, rotation);  // after deeper recursions returned remove the piece from the board in order to place it at another location or to skip it for the current row;
					piece->used = 0;  // adjust used indicator;
					nopen += piece->x_range[rotation];  // adjust number of free sites;
					for(short i=nused+1; i<10; ++i) {
						pieces[i].skip = 0;  // reset skip of subsequent pieces so they can be used in the current row (their skip was set in deeper recursions); preceding pieces, if skipped, remain unchanged as their combination with other pieces was already checked in higher recursion levels (only 'forward' (or better 'downward') generation of combinations, no double generation);
					}
				}
			}
		}


		// repeat the above procedure for version B; for explanations/comments see the part for version A;
		piece->x_range = piece->x_range_B;
		if(piece->x_range[rotation] <= nopen) {

			piece->version = &(piece->B);
			for(short x=0; x<=x_max; ++x) {

				success = place_piece_on_board(board, piece, which_row, x, rotation);
				if(success == 0) {

					piece->used = 2000 + 100*rotation + 10*which_row + x;
					nopen -= piece->x_range[rotation];
					if(nopen == 0) {

						for(short i=0; i<10; ++i) {
							pieces[i].skip = 0;
						}
						iter_rows(board, pieces, which_row+1, fp_constellations);
					} else {

						iter_rows(board, pieces, which_row, fp_constellations);
					}
					remove_piece_from_board(board, piece, which_row, x, rotation);
					piece->used = 0;
					nopen += piece->x_range[rotation];
					for(short i=nused+1; i<10; ++i) {
						pieces[i].skip = 0;
					}
				}
			}
		}
	}

	// finally, do not use the current piece for the current row (so it can be used for subsequent rows);
	piece->skip = 1;
	iter_rows(board, pieces, which_row, fp_constellations);  // stay within the current row;
}



/**
 * Places a piece on the board, if possible.
 * 
 * @param board -- pointer to the two-dimensional array representing the board.
 * @param piece -- pointer to the piece that is to be placed.
 * @param which_row -- indicates which row the algorithm is currently working on.
 * @param x0 -- indicates the x-position of the left upper corner of the enclosing rectangle within which the piece will be placed.
 * @param rotation -- indicates the rotation of the piece.
 *
 * @return 0 if the piece was successfully placed, -1 if the piece could not be placed due to overlap with another piece.
 *
 */
int place_piece_on_board(short board[][5], struct Piece* piece, short const which_row, short const x0, short const rotation) {

	short tmp_board[4][4];  // temporal array for storing the new board values (so in case of an overlap already written board values don't need to be reset);
	short y_max, x_max;  // the actual y- and x-extent of the piece;

	if(rotation == 0) {  // 0 degrees;

		x_max = 2;
		y_max = piece->y_range;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				tmp_board[y][x] = board[which_row+y][x0+x] + (*(piece->version))[y][x];

				if(tmp_board[y][x] > 1) {  // check if pieces overlap;
					return -1;
				}
			}
		}

		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
			}
		}

	} else if(rotation == 1) {  // 90 degrees;

		x_max = piece->y_range;
		y_max = 2;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				tmp_board[y][x] = board[which_row+y][x0+x] + (*(piece->version))[x_max-1-x][y];

				if(tmp_board[y][x] > 1) {  // check if pieces overlap;
					return -1;
				}
			}
		}

		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
			}
		}

	} else if(rotation == 2) {  // 180 degrees;

		x_max = 2;
		y_max = piece->y_range;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				tmp_board[y][x] = board[which_row+y][x0+x] + (*(piece->version))[y_max-1-y][x_max-1-x];

				if(tmp_board[y][x] > 1) {  // check if pieces overlap;
					return -1;
				}
			}
		}

		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
			}
		}

	} else {  // rotation == 3, 270 degrees;

		x_max = piece->y_range;
		y_max = 2;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				tmp_board[y][x] = board[which_row+y][x0+x] + (*(piece->version))[x][y_max-1-y];

				if(tmp_board[y][x] > 1) {  // check if pieces overlap;
					return -1;
				}
			}
		}

		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
			}
		}
	}

	return 0;
}


/**
 * Removes a (already placed) piece from the board.
 *
 * @param board -- pointer to the two-dimensional array representing the board.
 * @param piece -- pointer to the piece that is to be placed.
 * @param which_row -- indicates which row the algorithm is currently working on.
 * @param x0 -- indicates the x-position of the left upper corner of the enclosing rectangle within which the piece will be placed.
 * @param rotation -- indicates the rotation of the piece.
 *
 * @see place_piece_on_board 
 *
 */
void remove_piece_from_board(short board[][5], struct Piece* piece, short const which_row, short const x0, short const rotation) {

	short y_max, x_max;

	if(rotation == 0) {

		x_max = 2;
		y_max = piece->y_range;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] -= (*(piece->version))[y][x];
			}
		}

	} else if(rotation == 1) {

		x_max = piece->y_range;
		y_max = 2;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] -= (*(piece->version))[x_max-1-x][y];
			}
		}

	} else if(rotation == 2) {

		x_max = 2;
		y_max = piece->y_range;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] -= (*(piece->version))[y_max-1-y][x_max-1-x];
			}
		}

	} else {  // rotation == 3;

		x_max = piece->y_range;
		y_max = 2;
		for(short x=0; x<x_max; ++x) {
			for(short y=0; y<y_max; ++y) {

				board[which_row+y][x0+x] -= (*(piece->version))[x][y_max-1-y];
			}
		}
	}
}


/**
 * Creates the pieces.
 *
 * @param pieces -- pointer to the array of all pieces.
 *
 * @see struct Piece
 *
 */
void create_pieces(struct Piece* pieces) {

	// lightgreen
	pieces[0].y_range = 3;
	pieces[0].x_range_A[0] = 1;
	pieces[0].x_range_A[1] = 3;
	pieces[0].x_range_A[2] = 2;
	pieces[0].x_range_A[3] = 1;
	pieces[0].A[0][0] = 1; pieces[0].A[0][1] = 0;
	pieces[0].A[1][0] = 1; pieces[0].A[1][1] = 0;
	pieces[0].A[2][0] = 1; pieces[0].A[2][1] = 1;

	pieces[0].x_range_B[0] = 2;
	pieces[0].x_range_B[1] = 3;
	pieces[0].x_range_B[2] = 2;
	pieces[0].x_range_B[3] = 2;
	pieces[0].B[0][0] = 1; pieces[0].B[0][1] = 1;
	pieces[0].B[1][0] = 1; pieces[0].B[1][1] = 0;
	pieces[0].B[2][0] = 1; pieces[0].B[2][1] = 1;
	pieces[0].B[3][0] = 2; pieces[0].B[3][1] = 2;



	// darkgreen
	pieces[1].y_range = 3;
	pieces[1].x_range_A[0] = 1;
	pieces[1].x_range_A[1] = 3;
	pieces[1].x_range_A[2] = 1;
	pieces[1].x_range_A[3] = 1;
	pieces[1].A[0][0] = 1; pieces[1].A[0][1] = 0;
	pieces[1].A[1][0] = 1; pieces[1].A[1][1] = 1;
	pieces[1].A[2][0] = 1; pieces[1].A[2][1] = 0;
	pieces[1].A[3][0] = 2; pieces[1].A[3][1] = 2;

	pieces[1].x_range_B[0] = 1;
	pieces[1].x_range_B[1] = 3;
	pieces[1].x_range_B[2] = 2;
	pieces[1].x_range_B[3] = 2;
	pieces[1].B[0][0] = 1; pieces[1].B[0][1] = 0;
	pieces[1].B[1][0] = 1; pieces[1].B[1][1] = 1;
	pieces[1].B[2][0] = 1; pieces[1].B[2][1] = 1;
	pieces[1].B[3][0] = 2; pieces[1].B[3][1] = 2;



	// purple
	pieces[2].y_range = 3;
	pieces[2].x_range_A[0] = 2;
	pieces[2].x_range_A[1] = 3;
	pieces[2].x_range_A[2] = 1;
	pieces[2].x_range_A[3] = 1;
	pieces[2].A[0][0] = 1; pieces[2].A[0][1] = 1;
	pieces[2].A[1][0] = 1; pieces[2].A[1][1] = 0;
	pieces[2].A[2][0] = 1; pieces[2].A[2][1] = 0;
	pieces[2].A[3][0] = 2; pieces[2].A[3][1] = 2;

	pieces[2].x_range_B[0] = 2;
	pieces[2].x_range_B[1] = 3;
	pieces[2].x_range_B[2] = 1;
	pieces[2].x_range_B[3] = 2;
	pieces[2].B[0][0] = 1; pieces[2].B[0][1] = 1;
	pieces[2].B[1][0] = 1; pieces[2].B[1][1] = 1;
	pieces[2].B[2][0] = 1; pieces[2].B[2][1] = 0;
	pieces[2].B[3][0] = 2; pieces[2].B[3][1] = 2;



	// darkblue
	pieces[3].y_range = 3;
	pieces[3].x_range_A[0] = 1;
	pieces[3].x_range_A[1] = 3;
	pieces[3].x_range_A[2] = 1;
	pieces[3].x_range_A[3] = 1;
	pieces[3].A[0][0] = 1; pieces[3].A[0][1] = 0;
	pieces[3].A[1][0] = 1; pieces[3].A[1][1] = 1;
	pieces[3].A[2][0] = 1; pieces[3].A[2][1] = 0;
	pieces[3].A[3][0] = 2; pieces[3].A[3][1] = 2;

	pieces[3].x_range_B[0] = 2;
	pieces[3].x_range_B[1] = 3;
	pieces[3].x_range_B[2] = 2;
	pieces[3].x_range_B[3] = 2;
	pieces[3].B[0][0] = 1; pieces[3].B[0][1] = 1;
	pieces[3].B[1][0] = 1; pieces[3].B[1][1] = 0;
	pieces[3].B[2][0] = 1; pieces[3].B[2][1] = 1;
	pieces[3].B[3][0] = 2; pieces[3].B[3][1] = 2;



	// blue
	pieces[4].y_range = 4;
	pieces[4].x_range_A[0] = 1;
	pieces[4].x_range_A[1] = 4;
	pieces[4].x_range_A[2] = 2;
	pieces[4].x_range_A[3] = 1;
	pieces[4].A[0][0] = 1; pieces[4].A[0][1] = 0;
	pieces[4].A[1][0] = 1; pieces[4].A[1][1] = 0;
	pieces[4].A[2][0] = 1; pieces[4].A[2][1] = 0;
	pieces[4].A[3][0] = 1; pieces[4].A[3][1] = 1;

	pieces[4].x_range_B[0] = 2;
	pieces[4].x_range_B[1] = 4;
	pieces[4].x_range_B[2] = 1;
	pieces[4].x_range_B[3] = 2;
	pieces[4].B[0][0] = 1; pieces[4].B[0][1] = 1;
	pieces[4].B[1][0] = 1; pieces[4].B[1][1] = 0;
	pieces[4].B[2][0] = 1; pieces[4].B[2][1] = 1;
	pieces[4].B[3][0] = 1; pieces[4].B[3][1] = 0;



	// red
	pieces[5].y_range = 4;
	pieces[5].x_range_A[0] = 1;
	pieces[5].x_range_A[1] = 4;
	pieces[5].x_range_A[2] = 2;
	pieces[5].x_range_A[3] = 1;
	pieces[5].A[0][0] = 1; pieces[5].A[0][1] = 0;
	pieces[5].A[1][0] = 1; pieces[5].A[1][1] = 0;
	pieces[5].A[2][0] = 1; pieces[5].A[2][1] = 0;
	pieces[5].A[3][0] = 1; pieces[5].A[3][1] = 1;

	pieces[5].x_range_B[0] = 2;
	pieces[5].x_range_B[1] = 4;
	pieces[5].x_range_B[2] = 2;
	pieces[5].x_range_B[3] = 2;
	pieces[5].B[0][0] = 1; pieces[5].B[0][1] = 1;
	pieces[5].B[1][0] = 1; pieces[5].B[1][1] = 0;
	pieces[5].B[2][0] = 1; pieces[5].B[2][1] = 0;
	pieces[5].B[3][0] = 1; pieces[5].B[3][1] = 1;



	// orange
	pieces[6].y_range = 4;
	pieces[6].x_range_A[0] = 1;
	pieces[6].x_range_A[1] = 4;
	pieces[6].x_range_A[2] = 1;
	pieces[6].x_range_A[3] = 1;
	pieces[6].A[0][0] = 1; pieces[6].A[0][1] = 0;
	pieces[6].A[1][0] = 1; pieces[6].A[1][1] = 0;
	pieces[6].A[2][0] = 1; pieces[6].A[2][1] = 1;
	pieces[6].A[3][0] = 1; pieces[6].A[3][1] = 0;

	pieces[6].x_range_B[0] = 1;
	pieces[6].x_range_B[1] = 4;
	pieces[6].x_range_B[2] = 2;
	pieces[6].x_range_B[3] = 2;
	pieces[6].B[0][0] = 1; pieces[6].B[0][1] = 0;
	pieces[6].B[1][0] = 1; pieces[6].B[1][1] = 1;
	pieces[6].B[2][0] = 1; pieces[6].B[2][1] = 0;
	pieces[6].B[3][0] = 1; pieces[6].B[3][1] = 1;



	// lightblue
	pieces[7].y_range = 4;
	pieces[7].x_range_A[0] = 1;
	pieces[7].x_range_A[1] = 4;
	pieces[7].x_range_A[2] = 1;
	pieces[7].x_range_A[3] = 1;
	pieces[7].A[0][0] = 1; pieces[7].A[0][1] = 0;
	pieces[7].A[1][0] = 1; pieces[7].A[1][1] = 0;
	pieces[7].A[2][0] = 1; pieces[7].A[2][1] = 1;
	pieces[7].A[3][0] = 1; pieces[7].A[3][1] = 0;

	pieces[7].x_range_B[0] = 1;
	pieces[7].x_range_B[1] = 4;
	pieces[7].x_range_B[2] = 1;
	pieces[7].x_range_B[3] = 2;
	pieces[7].B[0][0] = 1; pieces[7].B[0][1] = 0;
	pieces[7].B[1][0] = 1; pieces[7].B[1][1] = 1;
	pieces[7].B[2][0] = 1; pieces[7].B[2][1] = 1;
	pieces[7].B[3][0] = 1; pieces[7].B[3][1] = 0;



	// rose
	pieces[8].y_range = 4;
	pieces[8].x_range_A[0] = 1;
	pieces[8].x_range_A[1] = 4;
	pieces[8].x_range_A[2] = 1;
	pieces[8].x_range_A[3] = 1;
	pieces[8].A[0][0] = 1; pieces[8].A[0][1] = 0;
	pieces[8].A[1][0] = 1; pieces[8].A[1][1] = 1;
	pieces[8].A[2][0] = 1; pieces[8].A[2][1] = 0;
	pieces[8].A[3][0] = 1; pieces[8].A[3][1] = 0;

	pieces[8].x_range_B[0] = 1;
	pieces[8].x_range_B[1] = 4;
	pieces[8].x_range_B[2] = 2;
	pieces[8].x_range_B[3] = 2;
	pieces[8].B[0][0] = 1; pieces[8].B[0][1] = 0;
	pieces[8].B[1][0] = 1; pieces[8].B[1][1] = 0;
	pieces[8].B[2][0] = 1; pieces[8].B[2][1] = 1;
	pieces[8].B[3][0] = 1; pieces[8].B[3][1] = 1;



	// yellow
	pieces[9].y_range = 4;
	pieces[9].x_range_A[0] = 2;
	pieces[9].x_range_A[1] = 4;
	pieces[9].x_range_A[2] = 1;
	pieces[9].x_range_A[3] = 1;
	pieces[9].A[0][0] = 1; pieces[9].A[0][1] = 1;
	pieces[9].A[1][0] = 1; pieces[9].A[1][1] = 0;
	pieces[9].A[2][0] = 1; pieces[9].A[2][1] = 0;
	pieces[9].A[3][0] = 1; pieces[9].A[3][1] = 0;

	pieces[9].x_range_B[0] = 2;
	pieces[9].x_range_B[1] = 4;
	pieces[9].x_range_B[2] = 1;
	pieces[9].x_range_B[3] = 2;
	pieces[9].B[0][0] = 1; pieces[9].B[0][1] = 1;
	pieces[9].B[1][0] = 1; pieces[9].B[1][1] = 1;
	pieces[9].B[2][0] = 1; pieces[9].B[2][1] = 0;
	pieces[9].B[3][0] = 1; pieces[9].B[3][1] = 0;
}