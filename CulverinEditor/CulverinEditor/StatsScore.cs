using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


class StatsScore : CulverinBehaviour
{
    uint enem_killed = 0; // enemies player has killed
    float puzzle_time = 0; // time wasted trying to solve puzzle
    bool in_puzzle = false; //boolean to track if we are solving the puzzle (to track time)
    uint puzzle_tries = 0; // tries used to solve the puzzle
    uint characters_used = 0; //counter for tracking how many characters you have used (1,2 or 3)
    //variables for Sergio Saez, to manage balancing though inspector
    public int total_enemies = 0; //total enemies in the level
    public int oneCoin_time_lvl = 0; //one coin time
    public int twoCoin_time_lvl = 0; //two coin time
    public int threeCoin_time_lvl = 0; //three coin time

    void Start()
    {
        Time.StartTimePlay();
    }

    void Update()
    {
        if(in_puzzle)
        {
            puzzle_time += Time.deltaTime;
        }
    }
    
    void StartPuzzle()
    {
        puzzle_time = 0;
        in_puzzle = true;
    }
    
    void KillEnemy()
    {
        enem_killed++;
    }

    void PuzzleTry()
    {
        puzzle_tries++;
    }

    void CharactUsed()
    {
        characters_used++;
    }

    void ResetScore()
    {
        enem_killed = 0;
        puzzle_time = 0;
        puzzle_tries = 0;
        characters_used = 0;
    }
    

}