import subprocess
import sys
import os

TESTS = [
    # Category 1: Basic Commands & Arguments
    ("Basic command", "ls\n", False),
    ("Command with arguments", "ls -la /tmp\n", False),
    ("Multiple spaces between args", "echo    hello     world\n", False),
    ("Only newline", "\n", False),
    ("Only spaces", "     \n", False),

    # Category 2: Quotes and Escaping
    ("Single quotes", "echo 'hello world'\n", False),
    ("Double quotes", "echo \"hello world\"\n", False),
    ("Escaped space", "echo hello\\ world\n", False),
    ("Mixed quotes", "echo \"a'b\" 'c\"d'\n", False),
    ("Adjacent quotes (a\"b c\"d)", "echo a\"b c\"d\n", False),
    ("Empty quotes", "echo '' \"\"\n", False),
    ("Backslash in double quotes", "echo \"\\\"foo\\\"\"\n", False),
    ("Unclosed single quote (error)", "echo 'abc\n", True),
    ("Unclosed double quote (error)", "echo \"abc\n", True),

    # Category 3: Comments
    ("Comment after command", "echo hello # this is a comment\n", False),
    ("Comment only line", "# comment only\n", False),

    # Category 4: Sequences & Background
    ("Semicolon 2 commands", "echo 1 ; echo 2\n", False),
    ("Semicolon 3 commands", "echo 1 ; echo 2 ; echo 3\n", False),
    ("Trailing semicolon", "echo 1 ;\n", False),
    ("Background operator &", "echo 1 &\n", False),
    ("Background between commands", "echo 1 & echo 2\n", False),

    # Category 5: Pipelines
    ("Simple pipeline", "cat file | wc -l\n", False),
    ("3-stage pipeline", "cat | grep a | wc -l\n", False),
    ("4-stage pipeline", "cat | grep a | sort | uniq\n", False),
    ("Pipeline without spaces", "cat|wc\n", False),

    # Category 6: Logical AND / OR
    ("Logical AND (&&)", "echo 1 && echo 2\n", False),
    ("Logical OR (||)", "echo 1 || echo 2\n", False),
    ("AND then OR", "echo 1 && echo 2 || echo 3\n", False),
    ("OR then AND", "echo 1 || echo 2 && echo 3\n", False),

    # Category 7: Redirections
    ("Redirect OUT (>)", "echo hello > out.txt\n", False),
    ("Redirect IN (<)", "cat < in.txt\n", False),
    ("Redirect APPEND (>>)", "echo hello >> append.txt\n", False),
    ("Redirect IN and OUT", "cat < in.txt > out.txt\n", False),
    ("Redirect with flags mixed", "cat < in.txt -n > out.txt\n", False),
    ("Redirect at beginning", "> out.txt echo hello\n", False),
    ("Standalone redirect", "> out.txt\n", False),
    ("Multiple standalone redirects", "> out.txt < in.txt\n", False),
    ("Missing redir filename (error)", "cat >\n", True),

    # Category 8: Subshells (Parentheses)
    ("Simple subshell", "(echo 1)\n", False),
    ("Subshell with list", "(echo 1 ; echo 2)\n", False),
    ("Nested subshells", "((echo 1))\n", False),
    ("Triple nested subshells", "(((echo 1)))\n", False),
    ("Subshell in pipeline", "(echo 1 ; echo 2) | wc\n", False),
    ("Subshell with redirection", "(echo 1) > out.txt\n", False),
    ("Subshell with trailing semicolon", "(echo 1 ;)\n", False),
    ("Subshell AND subshell", "(echo 1) && (echo 2)\n", False),
    ("Empty subshell () (error)", "()\n", True),
    ("Unclosed subshell (echo 1 (error)", "(echo 1\n", True),
    ("Stray closing bracket echo 1) (error)", "echo 1)\n", True),
    ("Stray bracket ) (error)", ")\n", True),

    # Category 9: Backslash & Newlines
    ("Escaped newline continuation", "echo hello \\\nworld\n", False),
    ("Double backslash command", "echo \\\\\n", False),
    ("Double backslash with word", "echo \\\\a\n", False),
]

def run_tests():
    total = len(TESTS)
    passed = 0
    failed = 0
    asan_errors = 0
    results = []

    print("=" * 70)
    print(f"RUNNING COMPLETE SYSTEM VERIFICATION ({total} TESTS)")
    print("=" * 70)

    for name, inp, is_error_expected in TESTS:
        proc = subprocess.Popen(
            ["./mysh"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        try:
            stdout, stderr = proc.communicate(inp, timeout=3)
        except subprocess.TimeoutExpired:
            proc.kill()
            stdout, stderr = proc.communicate()
            print(f"[FAIL - TIMEOUT/HANG] {name}")
            results.append((name, "TIMEOUT", inp, stdout, stderr))
            failed += 1
            continue

        all_output = stdout + "\n" + stderr
        has_asan = "AddressSanitizer" in all_output or "runtime error" in all_output
        has_syntax_err = (
            "parser:" in stderr or
            "lexer:" in stderr or
            "error" in stderr.lower() or
            "syntax" in stderr.lower() or
            "unexpected" in stderr.lower()
        )

        if has_asan:
            print(f"[FAIL - ASAN/CRASH] {name}")
            results.append((name, "ASAN", inp, stdout, stderr))
            asan_errors += 1
            failed += 1
        elif is_error_expected and not has_syntax_err:
            print(f"[FAIL - EXPECTED ERROR NOT REPORTED] {name}")
            results.append((name, "NO_ERROR", inp, stdout, stderr))
            failed += 1
        elif not is_error_expected and has_syntax_err:
            print(f"[FAIL - UNEXPECTED ERROR] {name}: {stderr.strip()}")
            results.append((name, "UNEXPECTED_ERROR", inp, stdout, stderr))
            failed += 1
        else:
            print(f"[PASS] {name}")
            passed += 1

    print("\n" + "=" * 70)
    print(f"SUMMARY: Passed: {passed}/{total} | Failed: {failed}/{total} | ASan Leaks: {asan_errors}")
    print("=" * 70)

    if results:
        print("\n--- DETAILED FAILURE REPORT ---")
        for name, reason, inp, out, err in results:
            print(f"\n[-] TEST: {name} (Reason: {reason})")
            print(f"    Input: {inp!r}")
            if err:
                print(f"    Stderr: {err.strip()}")
            if out:
                lines = [l for l in out.splitlines() if any(k in l for k in ["COMMAND", "SUBSHELL", "SEQ", "PIPE", "AND", "OR", "redir", "Token type"])]
                print(f"    AST/Tokens: {lines[:6]}")

    return failed

if __name__ == "__main__":
    sys.exit(run_tests())
