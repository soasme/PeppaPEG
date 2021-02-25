References
==========

.. doxygendefine:: P4_FLAG_SQUASHED
.. doxygendefine:: P4_FLAG_LIFTED
.. doxygendefine:: P4_FLAG_TIGHT
.. doxygendefine:: P4_FLAG_SPACED
.. doxygendefine:: P4_FLAG_SCOPED

.. doxygendefine:: P4_DEFAULT_RECURSION_LIMIT

.. doxygentypedef:: P4_ExpressionFlag
.. doxygentypedef:: P4_Position
.. doxygentypedef:: P4_Rune
.. doxygentypedef:: P4_String
.. doxygentypedef:: P4_RuleID
.. doxygentypedef:: P4_RuneRange

.. doxygenenum:: P4_ExpressionKind
.. doxygenenum:: P4_Error

.. doxygenstruct:: P4_Slice
   :members:

.. doxygenstruct:: P4_Frame
   :members:

.. doxygenstruct:: P4_Grammar
   :members:

.. doxygenstruct:: P4_Expression
   :members:

.. doxygenstruct:: P4_Source
   :members:

.. doxygenstruct:: P4_Token
   :members:

.. doxygentypedef:: P4_MatchCallback
.. doxygentypedef:: P4_ErrorCallback

.. doxygenfunction:: P4_Version

.. doxygenfunction:: P4_CreateLiteral
.. doxygenfunction:: P4_CreateRange
.. doxygenfunction:: P4_CreateReference
.. doxygenfunction:: P4_CreatePositive
.. doxygenfunction:: P4_CreateNegative
.. doxygenfunction:: P4_CreateSequence
.. doxygenfunction:: P4_CreateSequenceWithMembers
.. doxygenfunction:: P4_CreateChoice
.. doxygenfunction:: P4_CreateChoiceWithMembers
.. doxygenfunction:: P4_CreateRepeatMinMax
.. doxygenfunction:: P4_CreateRepeatMin
.. doxygenfunction:: P4_CreateRepeatMax
.. doxygenfunction:: P4_CreateRepeatExact
.. doxygenfunction:: P4_CreateZeroOrOnce
.. doxygenfunction:: P4_CreateZeroOrMore
.. doxygenfunction:: P4_CreateOnceOrMore
.. doxygenfunction:: P4_CreateBackReference

.. doxygenfunction:: P4_SetMember
.. doxygenfunction:: P4_SetReferenceMember
.. doxygenfunction:: P4_GetMembersCount
.. doxygenfunction:: P4_GetMember

.. doxygenfunction:: P4_DeleteExpression
.. doxygenfunction:: P4_PrintExpression

.. doxygenfunction:: P4_SetRuleID
.. doxygenfunction:: P4_IsRule

.. doxygenfunction:: P4_IsSquashed
.. doxygenfunction:: P4_IsLifted
.. doxygenfunction:: P4_IsTight
.. doxygenfunction:: P4_IsScoped
.. doxygenfunction:: P4_IsSpaced
.. doxygenfunction:: P4_SetExpressionFlag
.. doxygenfunction:: P4_SetSquashed
.. doxygenfunction:: P4_SetLifted
.. doxygenfunction:: P4_SetTight
.. doxygenfunction:: P4_SetSpaced
.. doxygenfunction:: P4_SetScoped

.. doxygenfunction:: P4_AddLiteral
.. doxygenfunction:: P4_AddNumeric
.. doxygenfunction:: P4_AddRange
.. doxygenfunction:: P4_AddReference
.. doxygenfunction:: P4_AddPositive
.. doxygenfunction:: P4_AddNegative
.. doxygenfunction:: P4_AddSequence
.. doxygenfunction:: P4_AddSequenceWithMembers
.. doxygenfunction:: P4_AddChoice
.. doxygenfunction:: P4_AddChoiceWithMembers
.. doxygenfunction:: P4_AddZeroOrOnce
.. doxygenfunction:: P4_AddZeroOrMore
.. doxygenfunction:: P4_AddOnceOrMore
.. doxygenfunction:: P4_AddRepeatMin
.. doxygenfunction:: P4_AddRepeatMax
.. doxygenfunction:: P4_AddRepeatMinMax
.. doxygenfunction:: P4_AddRepeatExact

.. doxygenfunction:: P4_DeleteGrammarRule
.. doxygenfunction:: P4_GetGrammarRule
.. doxygenfunction:: P4_SetGrammarRuleFlag
.. doxygenfunction:: P4_GetRecursionLimit
.. doxygenfunction:: P4_SetRecursionLimit

.. doxygenfunction:: P4_CreateGrammar
.. doxygenfunction:: P4_DeleteGrammar

.. doxygenfunction:: P4_CreateSource
.. doxygenfunction:: P4_DeleteSource

.. doxygenfunction:: P4_GetSourceAst
.. doxygenfunction:: P4_GetSourcePosition
.. doxygenfunction:: P4_CopyTokenString

.. doxygenfunction:: P4_Parse

.. doxygenfunction:: P4_HasError
.. doxygenfunction:: P4_GetError
.. doxygenfunction:: P4_GetErrorMessage

.. doxygenfunction:: P4_CreateToken
.. doxygenfunction:: P4_DeleteToken
.. doxygenfunction:: P4_GetTokenSlice
