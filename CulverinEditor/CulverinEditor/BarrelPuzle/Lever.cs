using System;
using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class Lever : CulverinBehaviour
{
    // Lines-----------------------------
    public GameObject Puzzle_line_1;
    public GameObject Puzzle_line_2;
    public GameObject Puzzle_line_3;
    public GameObject Puzzle_line_4;
    public GameObject Puzzle_line_5;
    public GameObject Puzzle_line_6;

    //Lists ---------------------------------------
    private List<GameObject> line1;
    private List<GameObject> line2;
    private List<GameObject> line3;
    private List<GameObject> line4;
    private List<GameObject> line5;
    private List<GameObject> line6;

    // 
    public float speed_barrel = 5.0f;
    public float wheight_barrel = 20.0f;

    //time to wait, move second mode
    public float delay_second_mode = 6.0f;
    private float time = 0.0f;

    public int barrel_per_line = 6;
    public int number_lines = 6;
    public bool active_lever = false;

    private bool phase1 = false; // Set info all barrels
    private bool phase2 = false; // Move barrels mode.PUZZLE
    private bool phase_wait = false; // wait to move the other mode
    private bool phase3 = false; // Move barrels mode.FILLING
    private bool editmap = false; // editmap

    private int[,] map;

    void Start()
    {
        Puzzle_line_1 = GetLinkedObject("Puzzle_line_1");
        Puzzle_line_2 = GetLinkedObject("Puzzle_line_2");
        Puzzle_line_3 = GetLinkedObject("Puzzle_line_3");
        Puzzle_line_4 = GetLinkedObject("Puzzle_line_4");
        Puzzle_line_5 = GetLinkedObject("Puzzle_line_5");
        Puzzle_line_6 = GetLinkedObject("Puzzle_line_6");

        line1 = new List<GameObject>();
        line2 = new List<GameObject>();
        line3 = new List<GameObject>();
        line4 = new List<GameObject>();
        line5 = new List<GameObject>();
        line6 = new List<GameObject>();

        // Get All barrels from Puzzle.
        SetBarrels();

        // Desactivate all barrels, while the player dont stay in puzzle. 
        DesactivateBarrels(line1);
        DesactivateBarrels(line2);
        DesactivateBarrels(line3);
        DesactivateBarrels(line4);
        DesactivateBarrels(line5);
        DesactivateBarrels(line6);

        // Map
        map = new int[barrel_per_line, number_lines];
        // Testing --------------------------------------------
        for (int y = 0; y < number_lines; y++)
        {
            for (int x = 0; x < barrel_per_line; x++)
            {
                map[x, y] = 0;
            }
        }

        map[4, 0] = 1; 
        map[4, 1] = 1; 
        map[4, 2] = 1;
        map[4, 3] = 1;
        map[5, 3] = 1;
        map[5, 4] = 1;
        map[5, 5] = 1;
        map[5, 6] = 1;
        // -------------------------------------------------------
    }
    void Update()
    {
        if (active_lever)
        {
            if (!phase1) // Set info all barrels
            {
                SetInfo(line1);
                SetInfo(line2);
                SetInfo(line3);
                SetInfo(line4);
                SetInfo(line5);
                SetInfo(line6);
                phase1 = true;
            }
            if (!phase2) // Move barrels mode.PUZZLE
            {
                MoveBarrels(line1);
                MoveBarrels(line2);
                MoveBarrels(line3);
                MoveBarrels(line4);
                MoveBarrels(line5);
                MoveBarrels(line6);
                phase3 = true;
                phase2 = true;
                phase_wait = true;
                time = Time.realtimeSinceStartup;

            }
            if (phase_wait) // wait to move the other mode
            {
                // Wait delay to move other barrels
                float time_transcured = Time.realtimeSinceStartup + delay_second_mode;
                if (time_transcured >= time)
                {
                    phase3 = false;
                    phase_wait = false;
                }
            }
            if (!phase3) // Move barrels mode.FILLING
            {
                MoveBarrels(line1, true);
                MoveBarrels(line2, true);
                MoveBarrels(line3, true);
                MoveBarrels(line4, true);
                MoveBarrels(line5, true);
                MoveBarrels(line6, true);
                phase3 = true;
                editmap = true;
            }
            if (editmap)
            {

            }
        }
    }

    void MoveBarrels(List<GameObject> list, bool isfilling = false)
    {
        for (int i = 0; i < list.Count; i++)
        {
            if (list[i].GetComponent<BarrelFall>().IsPuzzleMode())
            {
                list[i].SetActive(true);
            }
            else if (isfilling)
            {
                list[i].SetActive(true);
            }
        }
    }

    void DesactivateBarrels(List<GameObject> list)
    {
        for (int i = 0; i < list.Count; i++)
        {
            list[i].SetActive(false);
        }
    }

    void SetInfo(List<GameObject> list)
    {
        for (int y = 0; y < number_lines; y++)
        {
            int count_barrel = barrel_per_line - 1;
            for (int x = barrel_per_line - 1; x >= 0; x--)
            {
                if (map[x, y] == 1)
                {
                    list[count_barrel--].GetComponent<BarrelFall>().SetData(speed_barrel, wheight_barrel, x, y, 10, BarrelFall.ModeBarrel.PUZZLE);
                }
            }
            for (int x = barrel_per_line - 1; x >= 0; x--)
            {
                if (map[x, y] == 1)
                {
                    list[count_barrel--].GetComponent<BarrelFall>().SetData(speed_barrel, wheight_barrel, x, y, 10, BarrelFall.ModeBarrel.FILLING);
                }
            }
        }
    }

    void SetBarrels()
    {
        bool stop = false;
        int count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_1.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line1.Add(temp);
        }
        stop = false;
        count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_2.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line2.Add(temp);
        }
        stop = false;
        count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_3.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line3.Add(temp);
        }
        stop = false;
        count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_4.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line4.Add(temp);
        }
        stop = false;
        count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_5.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line5.Add(temp);
        }
        stop = false;
        count = barrel_per_line;
        while (stop == false)
        {
            GameObject temp = Puzzle_line_6.GetChildByTagIndex("barrel", count);
            if (temp == null)
                stop = true;
            else
                line6.Add(temp);
        }
    }

}