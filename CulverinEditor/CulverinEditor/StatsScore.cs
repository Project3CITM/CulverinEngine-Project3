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


    static public void Start()
    {
        Time.StartTimePlay();
    }

    static public void StartPuzzle()  //NEEDS TO BE CALLED ONCE WE START PUZZLE
    {
        puzzle_time = 0;
        in_puzzle = true;
    }

    static public float PuzzleTimer()
    {
        in_puzzle = false;
        return puzzle_time;
    }

    static public void KillEnemy()
    {
        enem_killed++;
    }

    static public void PuzzleTry()
    {
        puzzle_tries++;
    }

    static public void CharactUsed()
    {
        characters_used++;
    }

    static public float GetTime()
    {
        return Time.timePlay;
    }

    static public void ResetScore()
    {
       enem_killed = 0;
       puzzle_time = 0;
       puzzle_tries = 0;
       characters_used = 0;
    }
}

class StatsScore : CulverinBehaviour
{
    static public void Update()
    {
        if (Data.in_puzzle)
        {
            Data.puzzle_time += Time.deltaTime;
        }
    }
}