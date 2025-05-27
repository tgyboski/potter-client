window.addEventListener("DOMContentLoaded", () => {
  const root = document.getElementById("root");
  console.log(document);
  if (root) {
    root.textContent = "teste";
    root.style.backgroundColor = "red";
  } else {
    console.error('Elemento com id "root" não encontrado');
  }
});
  