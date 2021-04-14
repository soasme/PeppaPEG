Change Log
===========

Not Yet Released
----------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.11.0..HEAD>.

* [Feature]: Support Unicode Character Categories for P4_Range. `#65 <https://github.com/soasme/PeppaPEG/pull/65>`_, `#67 <https://github.com/soasme/PeppaPEG/pull/67>`_

1.11.0 (8 Apr, 2021)
---------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.10.0..v1.11.0>.

* [Feature]: Allow setting arbitrary number of @spaced rules. `#66 <https://github.com/soasme/PeppaPEG/pull/66>`_
* [Feature]: Add P4_AcquireSourceAst. `#64 <https://github.com/soasme/PeppaPEG/pull/64>`_
* [Example]: Add tutjson page to docs. `#63 <https://github.com/soasme/PeppaPEG/pull/63>`_
* [Feature]: Add P4_ResetSource. `#62 <https://github.com/soasme/PeppaPEG/pull/62>`_
* [Feature]: Add P4_InspectSourceAst. `#61 <https://github.com/soasme/PeppaPEG/pull/61>`_
* [Feature]: Customize malloc/free/realloc. `#60 <https://github.com/soasme/PeppaPEG/pull/60>`_

1.10.0 (8 Apr, 2021)
---------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.9.0..v1.10.0>.

* [Feature]: Support `.` (maps to [\u{1}-\u{10ffff}]). `#59 <https://github.com/soasme/PeppaPEG/pull/59>`_
* [Example]: Refactor the JSON example to use PEG API. `#58 <https://github.com/soasme/PeppaPEG/pull/58>`_
* [Feature]: Jsonify token AST using grammar rule name. `#57 <https://github.com/soasme/PeppaPEG/pull/57>`_
* [Feature]: Added function: `P4_LoadGrammar`. `#56 <https://github.com/soasme/PeppaPEG/pull/56>`_

1.9.0 (1 Apr, 2021)
-------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.8.0..v1.9.0>.

* [Test]: Enable Valgrind for gcc check. `#55 <https://github.com/soasme/PeppaPEG/pull/55>`_
* [Feature]: Support specifying a name for a grammar rule. `#53 <https://github.com/soasme/PeppaPEG/pull/53>`_
* [Feature]: Added Lineno and Col Offset for `P4_Tokens`. `#52 <https://github.com/soasme/PeppaPEG/pull/52>`_
* [Feature]: Support parsing a subset of source input (`P4_SetSourceSlice`). `#51 <https://github.com/soasme/PeppaPEG/pull/51/>`_
* [Feature]: Added a new flag: `P4_FLAG_NON_TERMINAL`. `#50 <https://github.com/soasme/PeppaPEG/pull/50>`_

1.8.0 (25 Mar, 2021)
--------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.7.0..v1.8.0>.

* [Bugfix]: Support non-ASCII case insensitive literal match. `#48 <https://github.com/soasme/PeppaPEG/pull/48>`_ `#49 <https://github.com/soasme/PeppaPEG/pull/49>`_
* [Feature]: Allow user setting Userdata for P4_Tokens. `#47 <https://github.com/soasme/PeppaPEG/pull/47>`_
* [Feature]: Print source ast in JSON format. `#46 <https://github.com/soasme/PeppaPEG/pull/46>`_

1.7.0 (10 Mar, 2021)
--------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.6.0..v1.7.0>.

* [Enhancement]: Provided `P4_GetErrorString`. `#45 <https://github.com/soasme/PeppaPEG/pull/45>`_
* [Feature]: Added Join. `#44 <https://github.com/soasme/PeppaPEG/pull/44>`_
* [Example]: Wrote an Calculator example in Peppa PEG. `#43 <https://github.com/soasme/PeppaPEG/pull/43>`_
* [Feature]: Added StartOfInput and EndOfInput. `#42 <https://github.com/soasme/PeppaPEG/pull/42>`_
* [Feature]: Added parameter `stride` for Range. `#41 <https://github.com/soasme/PeppaPEG/pull/41>`_

1.6.0 (4 Mar, 2021)
-------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.5.0..v1.6.0>.

* [Enhancement]: Replaced `token->expr` with `token->rule_id`. `#40 <https://github.com/soasme/PeppaPEG/pull/40>`_
* [Bugfix]: Insert no whitespace while spacing is ongoing. `#39 <https://github.com/soasme/PeppaPEG/pull/39>`_
* [Example]: JSON example parser passes full JSON test suite. `#37 <https://github.com/soasme/PeppaPEG/pull/37>`_
* [Enhancement]: Support Emoji 🐷 `#36 <https://github.com/soasme/PeppaPEG/pull/36>`_
* [Bugfix]: Return NULL if CreateExpression receives invalid parameters. `#35 <https://github.com/soasme/PeppaPEG/pull/35>`_
* [Test]: Build docs & tests using GitHub Actions. `#32 <https://github.com/soasme/PeppaPEG/pull/32>`_, `#34 <https://github.com/soasme/PeppaPEG/pull/34>`_


1.5.0 (28 Feb, 2021)
--------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.4.0..v1.5.0>.

* Added option `sensitive` For BackReference. `#31 <https://github.com/soasme/PeppaPEG/pull/31>`_.
* Full ANSI C support for peppapeg.c. `#30 <https://github.com/soasme/PeppaPEG/pull/30>`_.
* Generate Website for Documentation. `#29 <https://github.com/soasme/PeppaPEG/pull/29>`_.
  Check it out: <https://www.soasme.com/PeppaPEG/>.

1.4.0 (24 Feb, 2021)
--------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.3.0..v1.4.0>.

* Feature: Added `P4_ReplaceGrammarRule` and `P4_SetGrammarCallback`. `#28 <https://github.com/soasme/PeppaPEG/pull/28/>`_.
* Added some documentation for data structures.
* Example: Make JSON example RFC7159 compatible. `#25 <https://github.com/soasme/PeppaPEG/pull/25>`_.
* Bugfix: Replace int with `size_t`. `#22 <https://github.com/soasme/PeppaPEG/pull/22>`_, `#23 <https://github.com/soasme/PeppaPEG/pull/23>`_.
* Experiment: Add Python Binding. `#20 <https://github.com/soasme/PeppaPEG/pull/20>`_, `#24 <https://github.com/soasme/PeppaPEG/pull/24>`_.

1.3.0 (19 Feb, 2021)
---------------------

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.2.0..v1.3.0>.

* Feature: Added P4_GetRecursionLimit and P4_SetRecursionLimit. (`#19 <https://github.com/soasme/PeppaPEG/pull/19>`_).
* Performance Optimization: the performance is now 10x faster than 1.2.0. (`#15 <https://github.com/soasme/PeppaPEG/pull/15>`_, `#16 <https://github.com/soasme/PeppaPEG/pull/16>`_, `#17 <https://github.com/soasme/PeppaPEG/pull/17>`_, `#18 <https://github.com/soasme/PeppaPEG/pull/18>`_.
* Bugfix: Memory leak fixed. `#14 <https://github.com/soasme/PeppaPEG/pull/14>`_.
* Example: Write a JSON Parser using Peppa PEG. `#13 <https://github.com/soasme/PeppaPEG/pull/13>`_.
* Example: Write a Mustache Parser using Peppa PEG. `#12 <https://github.com/soasme/PeppaPEG/pull/12>`_.
* Bugfix: Raise P4_AdvanceError if repeated expression consumes no input. `#11 <https://github.com/soasme/PeppaPEG/pull/11>`_.

1.2.0 (15 Feb, 2021)
---------------------

* Add New Expression Kind: `P4_BackReference`. (#10)
* Change the return value to `P4_Error` for `P4_AddGrammarRule`. (#9)
* Added `P4_GetSourcePosition`. (#8)

1.1.0 (10 Feb, 2021)
---------------------

* C89 compliant. (#7)
* Add PUBLIC(P4_Error) P4_SetGrammarRuleFlag. (#6)
* Example: Write an INI Parser using Peppa PEG. (#5)
* Has/Get error and error message (#4)
* Turn Get/Set Whitespaces as PRIVATE functions. (#3)
* AddSequenceMembers/AddChoicMembers functions. (#2)
* Add ExpressionFlag Getter/Setter Functions. (#1)


1.0.0 (08 Feb, 2021)
---------------------

Kia Ora! This is the very fist release of Peppa PEG! It was strongly influenced by [Pest](https://pest.rs).
