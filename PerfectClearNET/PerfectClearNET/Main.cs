using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace PerfectClearNET {
    static class Interface {
        [DllImport("sfinder-dll.dll")]
        private static extern void action(string field, string queue, string hold, StringBuilder str, int len);

        public static string Process(string field, string queue, string hold) {
            StringBuilder sb = new StringBuilder(500);
            action(field, queue, hold, sb, sb.Capacity);
            return sb.ToString();
        }
    }

    public static class PerfectClear {
        private static readonly string[] MinoMap = new string[7] {
            "Z", "S", "L", "J", "T", "O", "I"
        };

        static PerfectClear() {}

        public static void Find(int[,] field, int[] queue, int current, int? hold) {
            string f = "";

            for (int i = 0; i < 24; i++)
                for (int j = 0; j < 10; j++)
                    f += (field[j,i] == 255)? '_' : 'X';

            string q = MinoMap[current];

            for (int i = 0; i < queue.Length; i++)
                q += MinoMap[queue[i]];

            string h = (hold == null) ? "E" : MinoMap[hold.Value];
            
            string result = Interface.Process(f, q, h);

            if (!result.Equals("-1")) {
                int a = 123;
            }
        }
    }
}
