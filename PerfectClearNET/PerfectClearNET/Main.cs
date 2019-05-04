using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace PerfectClearNET {
    static class Interface {
        [DllImport("sfinder-dll.dll")]
        private static extern void action(string field, string queue, string hold, int height, ref bool abort, StringBuilder str, int len);

        private static object locker = new object();
        private static bool abort = false;

        public static string Process(string field, string queue, string hold, int height) {
            StringBuilder sb = new StringBuilder(500);

            abort = true;

            lock (locker) {
                abort = false;
                action(field, queue, hold, height, ref abort, sb, sb.Capacity);

                if (abort) return "";
            }

            return sb.ToString();
        }
    }

    public static class PerfectClear {
        private static readonly string[] MinoMap = new string[7] {
            "Z", "S", "L", "J", "T", "O", "I"
        };

        public delegate void AbortedEventHandler();
        public static event AbortedEventHandler SearchAborted;

        public delegate void FinishedEventHandler(bool success, string result);
        public static event FinishedEventHandler SearchFinished;

        static PerfectClear() {}

        public static async void Find(int[,] field, int[] queue, int current, int? hold) {
            int c = 0;
            int t = -1;
            string f = "";

            for (int i = 19; i >= 0; i--)
                for (int j = 0; j < 10; j++) {
                    if (field[j, i] == 255) {
                        f += '_';
                        c += 1;
                    } else {
                        f += 'X';
                        if (t == -1) t = i + 1;
                    }
                }

            string q = MinoMap[current];

            for (int i = 0; i < queue.Length; i++)
                q += MinoMap[queue[i]];

            string h = (hold == null) ? "E" : MinoMap[hold.Value];

            if (c % 4 == 2) t += 1;

            string result = "";

            await Task.Run(() => {
                result = Interface.Process(f, q, h, t);

                if (result.Equals("")) {
                    SearchAborted?.Invoke();

                } else if (result.Equals("-1")) {
                    SearchFinished?.Invoke(false, result);

                } else {
                    SearchFinished?.Invoke(true, result);
                }
            });
        }
    }
}
