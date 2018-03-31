using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

static public class Data
{
    static public uint enem_killed = 0; // enemies player has killed
    static public float puzzle_time = 0; // time wasted trying to solve puzzle
    static public bool in_puzzle = false; //boolean to track if we are solving the puzzle (to track time)
    static public uint puzzle_tries = 0; // tries used to solve the puzzle
    static public uint characters_used = 0; //counter for tracking how many characters you have used (1,2 or 3)
    //variables for Sergio Saez, to manage balancing though inspector
    static public int total_enemies = 0; //total enemies in the level
    static public int oneCoin_time_lvl = 0; //one coin time
    static public int twoCoin_time_lvl = 0; //two coin time
    static public int threeCoin_time_lvl = 0; //three coin time
}

class StatsScore : CulverinBehaviour
{
    void Start()
    {
        Time.StartTimePlay();
    }

    void Update()
    {
        if(Data.in_puzzle)
        {
            Data.puzzle_time += Time.deltaTime;
        }
    }
    
    void StartPuzzle()
    {
        Data.puzzle_time = 0;
        Data.in_puzzle = true;
    }

    float PuzzleTimer()
    {
        Data.in_puzzle = false;
        return Data.puzzle_time;
    }

    void KillEnemy()
    {
        Data.enem_killed++;
    }

    uint GetEnemKilled()
    {
        return Data.enem_killed;
    }

    void PuzzleTry()
    {
        Data.puzzle_tries++;
    }

    uint GetPuzzleTries()
    {
        return Data.puzzle_tries;
    }

    void CharactUsed()
    {
        Data.characters_used++;
    }

    uint GetCaractUsed()
    {
        return Data.characters_used;
    }

    float GetTime()
    {
        return Time.timePlay;
    }

    void ResetScore()
    {
        Data.enem_killed = 0;
        Data.puzzle_time = 0;
        Data.puzzle_tries = 0;
        Data.characters_used = 0;
    }
    

}