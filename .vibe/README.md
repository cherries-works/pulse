# Usage of AI

At cherries.works, code quality comes first. We don't let AI write feature code, we use it only where it outperforms manual review, and we keep it fully transparent.

That's why the `.vibe/` folder is public: you can inspect every skill and see exactly how AI is used in this project.

We use Mistral AI - Vibe for two things:

- Vulnerability detection (white-box): The [pentesting skill](./skills/pentesting/SKILL.md) systematically walks the codebase to find security issues in the source itself, instead of probing from the outside.

- Edge-case testing: The [edge-case-testing skill](./skills/edge-case-testing/SKILL.md) checks inputs and their outputs, flagging crashes or mismatches with expected values.

To help AI understand what Pulse is and the philosophy behind it, we keep a [pulse-project skill](./skills/pulse-project/SKILL.md).

AI output is always checked before push/merge.

Suggestions and feedback are welcome.