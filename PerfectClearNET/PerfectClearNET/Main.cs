using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// Suppresses readonly suggestion
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Style", "IDE0044")]

// Suppresses naming rule violation
[assembly: System.Diagnostics.CodeAnalysis.SuppressMessage("Style", "IDE1006")]

namespace PerfectClearNET {
    static class Interface {
        private static bool abort = false;

        private delegate bool Callback();
        private static Callback AbortCallback;

        [DllImport("sfinder-dll.dll")]
        private static extern void set_abort(Callback func);

        private static object locker = new object();
        public static bool Running { get; private set; } = false;

        [DllImport("sfinder-dll.dll")]
        private static extern void action(string field, string queue, string hold, int height, bool swap, int combo, StringBuilder str, int len);

        static Interface() {
            AbortCallback = new Callback(Abort);
            set_abort(AbortCallback);
        }

        public static bool Abort() => abort;
        public static void SetAbort() {
            if (Running) abort = true;
        }

        public static string Process(string field, string queue, string hold, int height, bool swap, int combo, out long time) {
            StringBuilder sb = new StringBuilder(500);

            abort = true;

            lock (locker) {
                abort = false;

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                Running = true;

                action(field, queue, hold, height, swap, combo, sb, sb.Capacity);

                Running = false;

                stopwatch.Stop();
                time = stopwatch.ElapsedMilliseconds;
            }

            return sb.ToString();
        }
    }

    public static class Mino {
        public static readonly string[] ToChar = new string[7] {
            "S", "Z", "J", "L", "T", "O", "I"
        };

        public static readonly int[] FromFinder = new int[7] {
            4, 6, 3, 2, 0, 1, 5
        };
    }

    public class Operation {
        #pragma warning disable 0169
        public int Piece { get; private set; }
        public int X { get; private set; }
        public int Y { get; private set; }
        public int R { get; private set; }
        #pragma warning restore 0169

        public Operation(string input) {
            List<int> parsed = (from i in input.Split(',') select Convert.ToInt32(i)).ToList();

            Piece = Mino.FromFinder[parsed[0]];
            X = parsed[1];
            Y = 23 - parsed[2];
            R = parsed[3];
        }

        public override string ToString() => $"{Mino.ToChar[Piece]}={X},{Y},{R}";
    }

    public static class PerfectClear {
        public delegate void FinishedEventHandler(bool success);
        public static event FinishedEventHandler Finished;

        public static List<Operation> LastSolution = new List<Operation>();
        public static long LastTime = 0;

        public static bool Running { get => Interface.Running; }

        static PerfectClear() {}

        static ManualResetEvent abortWait;

        public static void Abort() {
            if (Interface.Running) {
                abortWait = new ManualResetEvent(false);

                Interface.SetAbort();

                abortWait.WaitOne();
            }
        }

        public static async void Find(int[,] field, int[] queue, int current, int? hold, bool swap, int combo) {
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

            if (t == -1) t = 2;

            string q = Mino.ToChar[current];

            for (int i = 0; i < queue.Length; i++)
                q += Mino.ToChar[queue[i]];

            string h = (hold == null) ? "E" : Mino.ToChar[hold.Value];

            if ((c % 4 == 2 && t % 2 == 0) || (c % 4 == 0 && t % 2 == 1)) t += 1;

            string result = "";

            await Task.Run(() => {
                result = Interface.Process(f, q, h, t, swap, combo, out long time);

                LastSolution = new List<Operation>();
                LastTime = time;

                bool solved = !result.Equals("-1");

                if (solved) {
                    foreach (string op in result.Split('|'))
                        if (op != "" && op != "0,0,0,0")
                            LastSolution.Add(new Operation(op));
                }

                Finished?.Invoke(solved);

                abortWait?.Set();
            });
        }
    }
}
