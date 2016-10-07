Object define:'clusterID' toBe: 490 asPrimitive

Object defineMethod:[|asPOP|
      ^self clusterID asPOP
];

Object defineMethod:[| getNthPOP: index|
!result <- ^self _getNthPOP: index;
result isntNA ifTrue: [:result <- result asPOP];
result]


