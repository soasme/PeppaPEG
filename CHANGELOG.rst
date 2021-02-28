Change Log
===========

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
