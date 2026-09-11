# English command help style

The English command help is written for a calculator user who needs a clear next step. Each entry should answer three questions in plain words:

1. What does the command do?
2. What are the important inputs and outputs?
3. What short command can the user run now?

The current catalog has 2,075 rows and 2,068 unique command names. The first cleanup pass rewrote 244 descriptions and expanded 40 examples. Every description is now at most 30 words. Entries with a callable signature have a runnable example; option names and constants may intentionally point to their parent command instead.

Use direct verbs such as `finds`, `draws`, `builds`, `checks`, and `returns`. Keep one idea per sentence. Put the result before a list of special cases. Use a technical word only when the command needs it, and explain it through the example when possible. Avoid dense phrases such as `infixed`, `respectively`, `iff`, and `corresponding`.

`tests/check-help.py` checks the row count, description length, plain wording, and example coverage. The English build workflow runs this check before the calculator toolchain build.
