#ifndef FILES_H
#define FILES_H
#include "types.h"

const char *getScoreTag(ScoreMode mode);
void saveScore(const char *filename, const char *playerName, int score);
void loadScores(const char *filename);
int loadScoresSection(const char *filename, ScoreMode mode, Score scores[]);
void sortScores(Score scores[], int count);
void writeAllScores(const char *filename,
                    Score easy[], int ne,
                    Score medium[], int nm,
                    Score hard[], int nh,
                    Score pvp[], int np);
void updateTopScoresOneFile(const char *filename,
                            ScoreMode mode,
                            const char *playerName,
                            int playerScore);
int getBestScore(const char *filename, ScoreMode mode);




#endif