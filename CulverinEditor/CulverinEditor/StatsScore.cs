using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

static public class StatsScore
{
    static public uint enem_killed = 0; // enemies player has killed
    static public uint puzzle_tries = 0; // tries used to solve the puzzle
    static public uint characters_alive = 0; //counter for tracking how many characters you have used (1,2 or 3)
    static public float time = 0.0f; //counter for tracking how many characters you have used (1,2 or 3)


 
    static public void KillEnemy()
    {
        enem_killed++;
    }

    static public void PuzzleTry()
    {
        puzzle_tries++;
    }

    static public void CharacterDead()
    {
        characters_alive++;
    }

    static public void GetTime()
    {
        time = Time.timePlay;
    }

    static public void ResetScore()
    {
        Debug.Log("Reset Score");
        time = 0.0f;
        enem_killed = 0;
        puzzle_tries = 0;
        characters_alive = 0;
        Time.StartTimePlay();
    }
}
