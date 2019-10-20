#ifndef _highscore_h
#define _highscore_h

struct score_list{
	char name[ 256 ];
	int score;
};

class Highscore{
public:

	Highscore();

	int maxScores();
	char * getName( int i );
	int getScore( int i );
	bool eligible( int w );

	void addScore( const char * name, int high );

	~Highscore();

protected:

	void sync();
	void sortScores();
	void readScores();
	void createList();

	score_list scores[ 10 ];
	char * filename;

};

#endif
