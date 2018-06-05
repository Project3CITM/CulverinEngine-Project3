using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

static public class StatsScore
{
    static public bool boss_dead = false;
    static public uint enemies_killed = 0; // enemies player has killed
    static public uint characters_alive = 3; //counter for tracking how many characters you have used (1,2 or 3)
    static public float time = 0.0f; //counter for tracking how many characters you have used (1,2 or 3)

    static public uint fountain_times = 0;
    static public float boss_time = 0.0f;

    static public void BossDead()
    {
        boss_dead = true;
    }
 
    static public void KillEnemy()
    {
        enemies_killed++;
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
        enemies_killed = 0;
        characters_alive = 3;
        Time.StartTimePlay();

        fountain_times = 0;
    }

    static public void Heal()
    {
        fountain_times++;
    }
}
