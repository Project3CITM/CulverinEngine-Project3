using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

static public class StatsScore
{
    static public bool boss_dead = false;
    static public uint enem_killed = 0; // enemies player has killed
    static public uint puzzle_tries = 0; // tries used to solve the puzzle
    static public uint characters_alive = 3; //counter for tracking how many characters you have used (1,2 or 3)
    static public float time = 0.0f; //counter for tracking how many characters you have used (1,2 or 3)

    static public uint fountain_times = 0;
    static public uint combat_points = 0;
    static public uint puzzle_hits = 0;

    static public void BossDead()
    {
        boss_dead = true;
        combat_points += 100;
    }
 
    static public void KillEnemy()
    {
        enem_killed++;
        combat_points += 50;
    }

    static public void PuzzleTry()
    {
        puzzle_tries++;
    }

    static public void CharacterDead()
    {
        characters_alive--;
    }

    static public void GetTime()
    {
        time = Time.timePlay;
    }

    static public void ResetScore()
    {
        boss_dead = false;
        time = 0.0f;
        enem_killed = 0;
        puzzle_tries = 0;
        characters_alive = 3;
        Time.StartTimePlay();

        combat_points = 0;
        fountain_times = 0;
        puzzle_hits = 0;
    }

    static public void Heal()
    {
        fountain_times++;
    }
}
