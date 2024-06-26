// BMP 영역의 문자들 (한 글자 중국어 문자가 대부분 여기에 속합니다)
let char1 = "汉"; // 중국어 '한'이라는 글자
console.log(char1.length); // 1

// BMP를 벗어나는 문자들 (고유 이모지, 고유한 유니코드 문자들)
let char2 = "𠀀"; // U+20000, 한자 '𠀀' (CJK Unified Ideographs Extension B)
console.log(char2.length); // 2

let char3 = "😀"; // U+1F600, 웃고 있는 얼굴 이모지
console.log(char3.length); // 2

