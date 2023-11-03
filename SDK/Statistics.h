#pragma once

class Statistics {
public:
	int kills;
	int assists;
	int deaths;
	int headshots;
	int score;

	Statistics() {
		kills = 0;
		assists = 0;
		deaths = 0;
		headshots = 0;
		score = 0;
	}

	~Statistics() {
		kills = 0xFFFFFFFF;
		assists = 0xFFFFFFFF;
		deaths = 0xFFFFFFFF;
		headshots = 0xFFFFFFFF;
		score = 0xFFFFFFFF;
	}
};