# Change Log

## 1.3.0 (19 Feb, 2021)

Code change: <https://github.com/soasme/PeppaPEG/compare/v1.2.0..v1.3.0>.

Feature: Added P4_GetRecursionLimit and P4_SetRecursionLimit. ([#19](https://github.com/soasme/PeppaPEG/pull/19)).
Performance Optimization: the performance is now 10x faster than 1.2.0. ([#15](https://github.com/soasme/PeppaPEG/pull/15), [#16](https://github.com/soasme/PeppaPEG/pull/16), [#17](https://github.com/soasme/PeppaPEG/pull/17), [#18](https://github.com/soasme/PeppaPEG/pull/18)).
Bufix: Memory leak fixed. ([#14](https://github.com/soasme/PeppaPEG/pull/14)).
Example: Write a JSON Parser using Peppa PEG. ([#13](https://github.com/soasme/PeppaPEG/pull/13)).
Example: Write a Mustache Parser using Peppa PEG. ([#12](https://github.com/soasme/PeppaPEG/pull/12)).
Bugfix: Raise P4_AdvanceError if repeated expression consumes no input. ([#11](https://github.com/soasme/PeppaPEG/pull/11)).

## 1.2.0 (15 Feb, 2021)

* Add New Expression Kind: `P4_BackReference`. (#10)
* Change the return value to `P4_Error` for `P4_AddGrammarRule`. (#9)
* Added `P4_GetSourcePosition`. (#8)

## 1.1.0 (10 Feb, 2021)

* C89 compliant. (#7)
* Add PUBLIC(P4_Error) P4_SetGrammarRuleFlag. (#6)
* Example: Write an INI Parser using Peppa PEG. (#5)
* Has/Get error and error message (#4)
* Turn Get/Set Whitespaces as PRIVATE functions. (#3)
* AddSequenceMembers/AddChoicMembers functions. (#2)
* Add ExpressionFlag Getter/Setter Functions. (#1)


## 1.0.0 (08 Feb, 2021)

Kia Ora! This is the very fist release of Peppa PEG! It was strongly influenced by [Pest](https://pest.rs).
